#include "generator.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "asmCommands.h"

// static ------------------------------------------------------------------------------------------

static const size_t kInitCapacityOfCodeArray = 4096;
static const int kAdditionalCapacityOfNameTable = 16;
static const char* const kNameOfEntryFunction = "_start";

static Operations GetOperationType(const char* const word);
static int FindVar(const TCodeGen* const cg, const char* id);
static void AddVar(TCodeGen* cg, const char* id);
static size_t FindFunc(const TCodeGen* const cg, const char* name);
static void AddFunc(TCodeGen* cg, const char* name);
static void CodeGenExpr(TCodeGen* cg, tNode* node);
static void CodeGenStmt(TCodeGen* cg, tNode* node);

namespace GenExpr {
    static void EmitNumber(TCodeGen* cg, tNode* node);

    static void EmitIdentifier(TCodeGen* cg, tNode* node);

    static void EmitCalling(TCodeGen* cg, tNode* node);

    static void EmitBinary(TCodeGen* cg, tNode* node);
    namespace Binary {
        static void EmitAdd(TCodeGen* cg);
        static void EmitSub(TCodeGen* cg);
        static void EmitMul(TCodeGen* cg);
        static void EmitDiv(TCodeGen* cg);
        static void EmitGreater(TCodeGen* cg);
        static void EmitGreaterOrEqual(TCodeGen* cg);
        static void EmitLess(TCodeGen* cg);
        static void EmitLessOrEqual(TCodeGen* cg);
        static void EmitIdentical(TCodeGen* cg);
        static void EmitNotIdentical(TCodeGen* cg);
    }
}

namespace GenStmt {
    static void EmitFunction(TCodeGen* cg, tNode* node);

    static void EmitOperation(TCodeGen* cg, tNode* node);
    namespace Operation {
        static void EmitSemicolon(TCodeGen* cg, tNode* node);
        static void EmitEqual(TCodeGen* cg, tNode* node);
        static void EmitPrintAscii(TCodeGen* cg, tNode* node);
        static void EmitPrintInt(TCodeGen* cg, tNode* node);
    }
}

static void CreatePrintAscii(TCodeGen* cg);
static void CreatePrintInt(TCodeGen* cg);

// global ------------------------------------------------------------------------------------------

void CodeGenCtor(TCodeGen* cg) {
    cg->size = 0;
    cg->stackOffset = 0;
    cg->labelCount = 0;

    cg->capacity = kInitCapacityOfCodeArray;
    cg->code = (uint8_t*)calloc(cg->capacity, sizeof(char));
    assert(cg->code);

    cg->varCount = 0;
    cg->vars = (TVariables*)calloc(kAdditionalCapacityOfNameTable, sizeof(TVariables));
    assert(cg->vars);

    cg->funcCount = 0;
    cg->funcs = (TFunctions*)calloc(kAdditionalCapacityOfNameTable, sizeof(TFunctions));
    assert(cg->funcs);
}

void CodeGenDtor(TCodeGen* cg) {
    free(cg->code);

    for (int i = 0; i != cg->varCount; ++i) {
        free(cg->vars[i].id);
    }
    free(cg->vars);

    for (int i = 0; i != cg->funcCount; ++i) {
        free(cg->funcs[i].name);
    }
    free(cg->funcs);
}

void AppendCode(TCodeGen* cg, const uint8_t* data, size_t len) {
    if (cg->size + len > cg->capacity) {
        cg->capacity *= 2;
        cg->code = (uint8_t*)realloc(cg->code, cg->capacity);
        assert(cg->code);
    }
    memcpy(cg->code + cg->size, data, len);
    cg->size += len;
}

void CodegenProgram(TCodeGen* cg, tNode* program, Elf64_Ehdr* ehdr) {
    nop(cg);
    CreatePrintAscii(cg);
    CreatePrintInt(cg);
    ehdr->e_entry += cg->size;

    AddFunc(cg, kNameOfEntryFunction);
    push_reg(cg, REG_BP);
    mov_reg_reg(cg, REG_BP, REG_SP);
    CodeGenStmt(cg, program);
    mov_reg_reg(cg, REG_SP, REG_BP);
    pop_reg(cg, REG_BP);
    mov_reg_imm32(cg, REG_AX, 60);
    mov_reg_imm32(cg, REG_DI, 0);
    syscall(cg);
}

// static ------------------------------------------------------------------------------------------

static Operations GetOperationType(const char* const word) {
         if (!strcmp(word, keyIf)) return If;               
    else if (!strcmp(word, keyAdd)) return Add; //
    else if (!strcmp(word, keySub)) return Sub; //
    else if (!strcmp(word, keyMul)) return Mul; //
    else if (!strcmp(word, keyDiv)) return Div; //
    else if (!strcmp(word, keySin)) return Sin;
    else if (!strcmp(word, keyCos)) return Cos; 
    else if (!strcmp(word, keySqrt)) return Sqrt;
    else if (!strcmp(word, keyWhile)) return While;         
    else if (!strcmp(word, keyEqual)) return Equal; //
    else if (!strcmp(word, keyPrintAscii)) return PrintAscii; //
    else if (!strcmp(word, keyPrintInt)) return PrintInt; // 
    else if (!strcmp(word, keyReturn)) return Return;
    else if (!strcmp(word, keyGreater)) return Greater; //
    else if (!strcmp(word, keyLess)) return Less; //
    else if (!strcmp(word, keySemicolon)) return Semicolon; //  
    else if (!strcmp(word, keyIdentical)) return Identical; //
    else if (!strcmp(word, keyLessOrEqual)) return LessOrEqual; //
    else if (!strcmp(word, keyNotIdentical)) return NotIdentical; //
    else if (!strcmp(word, keyGreaterOrEqual)) return GreaterOrEqual; //

    else return NoOperation;
}

static int FindVar(const TCodeGen* const cg, const char* id) {
    for (int i = 0; i != cg->varCount; ++i) {
        if (!strcmp(cg->vars[i].id, id)) {
            return cg->vars[i].offset;
        }
    }
    return -1;
}

static void AddVar(TCodeGen* cg, const char* id) {
    if (cg->varCount >= kAdditionalCapacityOfNameTable) {
        cg->vars = (TVariables*)realloc(
            cg->vars, (cg->varCount + kAdditionalCapacityOfNameTable) * sizeof(TVariables)
        );
        assert(cg->vars);
    }
    cg->vars[cg->varCount].id = strdup(id);
    cg->stackOffset += 8;
    cg->vars[cg->varCount].offset = cg->stackOffset;
    ++cg->varCount;
}

static size_t FindFunc(const TCodeGen* const cg, const char* name) {
    for (int i = 0; i < cg->funcCount; i++) {
        if (!strcmp(cg->funcs[i].name, name)) {
            return cg->funcs[i].addr;
        }
    }
    return 0;
}

static void AddFunc(TCodeGen* cg, const char* name) {
    if (cg->funcCount >= kAdditionalCapacityOfNameTable) {
        cg->funcs = (TFunctions*)realloc(
            cg->funcs, (cg->funcCount + kAdditionalCapacityOfNameTable) * sizeof(TFunctions)
        );
        assert(cg->funcs);
    }
    cg->funcs[cg->funcCount].name = strdup(name);
    cg->funcs[cg->funcCount].addr = cg->size;
    ++cg->funcCount;
}

static void CodeGenExpr(TCodeGen* cg, tNode* node) {
    switch (node->type) {
        case Number:        GenExpr::EmitNumber(cg, node);       break;
        case Identifier:    GenExpr::EmitIdentifier(cg, node);   break;        
        case Binary:        GenExpr::EmitBinary(cg, node);       break;
        case Calling:       GenExpr::EmitCalling(cg, node);      break;
        
        default: {
            fprintf(stderr, "Unknown expression type\n");
            exit(1);
        }
    }
}

static void CodeGenStmt(TCodeGen* cg, tNode* node) {
    if (!node) {
        return;
    }
    switch (node->type) {
        case Function:      GenStmt::EmitFunction(cg, node);        break;
        case Operation:     GenStmt::EmitOperation(cg, node);       break;

        default: {
            fprintf(stderr, "Unknown node type\n");
            exit(1);
        }
    }
}

static void GenExpr::EmitNumber(TCodeGen* cg, tNode* node) {
    mov_reg_imm32(cg, REG_AX, atoi(node->value));
}

static void GenExpr::EmitIdentifier(TCodeGen* cg, tNode* node) {
    int offset = FindVar(cg, node->value);
    if (offset == -1) {
        fprintf(stderr, "Undefined variable\n");
        exit(1);
    }
    mov_reg_mem(cg, REG_AX, -offset);
}

static void GenExpr::EmitBinary(TCodeGen* cg, tNode* node) {
    CodeGenExpr(cg, node->left);
    push_reg(cg, REG_AX);

    CodeGenExpr(cg, node->right);
    pop_reg(cg, REG_BX);
    
    switch (GetOperationType(node->value)) {
        case Add:               GenExpr::Binary::EmitAdd(cg);               break;
        case Sub:               GenExpr::Binary::EmitSub(cg);               break;
        case Mul:               GenExpr::Binary::EmitMul(cg);               break;
        case Div:               GenExpr::Binary::EmitDiv(cg);               break;
        case Greater:           GenExpr::Binary::EmitGreater(cg);           break;
        case GreaterOrEqual:    GenExpr::Binary::EmitGreaterOrEqual(cg);    break;
        case Less:              GenExpr::Binary::EmitLess(cg);              break;
        case LessOrEqual:       GenExpr::Binary::EmitLessOrEqual(cg);       break;
        case Identical:         GenExpr::Binary::EmitIdentical(cg);         break;
        case NotIdentical:      GenExpr::Binary::EmitNotIdentical(cg);      break;

        default: {
            fprintf(stderr, "Unknown binary operator\n");
            exit(1);
        }
    }
}

static void GenExpr::Binary::EmitAdd(TCodeGen* cg) {
    add_reg_reg(cg, REG_AX, REG_BX); 
}

static void GenExpr::Binary::EmitSub(TCodeGen* cg) {
    sub_reg_reg(cg, REG_AX, REG_BX);
}

static void GenExpr::Binary::EmitMul(TCodeGen* cg) {
    imul_reg_reg(cg, REG_AX, REG_BX);
}

static void GenExpr::Binary::EmitDiv(TCodeGen* cg) {
    xor_reg_reg(cg, REG_DX, REG_DX);
    xchg_reg_reg(cg, REG_AX, REG_BX);
    idiv_reg(cg, REG_BX);
}

static void GenExpr::Binary::EmitGreater(TCodeGen* cg) {
    cmp_reg_reg(cg, REG_AX, REG_BX);
    setg_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);
}

static void GenExpr::Binary::EmitGreaterOrEqual(TCodeGen* cg) {
    cmp_reg_reg(cg, REG_AX, REG_BX);
    setge_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);
}

static void GenExpr::Binary::EmitLess(TCodeGen* cg) {
    cmp_reg_reg(cg, REG_AX, REG_BX);
    setl_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);
}

static void GenExpr::Binary::EmitLessOrEqual(TCodeGen* cg) {
    cmp_reg_reg(cg, REG_AX, REG_BX);
    setle_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);
}

static void GenExpr::Binary::EmitIdentical(TCodeGen* cg) {
    cmp_reg_reg(cg, REG_AX, REG_BX);
    sete_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);
}

static void GenExpr::Binary::EmitNotIdentical(TCodeGen* cg) {
    cmp_reg_reg(cg, REG_AX, REG_BX);
    setne_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);
}

static void GenExpr::EmitCalling(TCodeGen* cg, tNode* node) {
    push_reg(cg, REG_DI);
    push_reg(cg, REG_SI);
    tNode* arg = node->left;
    if (arg) {
        CodeGenExpr(cg, arg);
        mov_reg_reg(cg, REG_DI, REG_AX);
        arg = arg->left;
        if (arg) {
            CodeGenExpr(cg, arg);
            mov_reg_reg(cg, REG_SI, REG_AX);
        }
    }
    size_t addr = FindFunc(cg, node->value);
    if (!addr) {
        fprintf(stderr, "Undefined function\n");
        exit(1);
    }
    call_rel32(cg, addr - (cg->size + 5)); // 5 is the size of the call_rel32
    pop_reg(cg, REG_SI);
    pop_reg(cg, REG_DI);
}

static void GenStmt::EmitFunction(TCodeGen* cg, tNode* node) {
    // TODO in process
}

static void GenStmt::EmitOperation(TCodeGen* cg, tNode* node) {
    switch (GetOperationType(node->value)) {
        case Semicolon:     GenStmt::Operation::EmitSemicolon(cg, node);        break;
        case Equal:         GenStmt::Operation::EmitEqual(cg, node);            break;
        case PrintAscii:    GenStmt::Operation::EmitPrintAscii(cg, node);       break;
        case PrintInt:      GenStmt::Operation::EmitPrintInt(cg, node);         break;

        default: {
            fprintf(stderr, "Unknown operation\n");
            exit(1);
        }
    }
}

static void GenStmt::Operation::EmitSemicolon(TCodeGen* cg, tNode* node) {
    CodeGenStmt(cg, node->left);
    CodeGenStmt(cg, node->right);
}

static void GenStmt::Operation::EmitEqual(TCodeGen* cg, tNode* node) {
    CodeGenExpr(cg, node->right);
    int offset = FindVar(cg, node->left->value);
    if (offset == -1) {
        AddVar(cg, node->left->value);
        offset = cg->vars[cg->varCount - 1].offset;
    }
    mov_mem_reg(cg, -offset, REG_AX);
}

static void GenStmt::Operation::EmitPrintAscii(TCodeGen* cg, tNode* node) {
    CodeGenExpr(cg, node->left);

    size_t printAsciiAddr = FindFunc(cg, keyPrintAscii);
    if (!printAsciiAddr) {
        fprintf(stderr, "Function %s not found\n", keyPrintAscii);
        exit(1);
    }

    mov_reg_reg(cg, REG_DI, REG_AX);

    push_reg(cg, REG_DI);
    push_reg(cg, REG_SI);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_CX);

    sub_reg_imm32(cg, REG_SP, 8);
    call_rel32(cg, printAsciiAddr - (cg->size + 5));
    add_reg_imm32(cg, REG_SP, 8);

    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_SI);
    pop_reg(cg, REG_DI);
}

static void GenStmt::Operation::EmitPrintInt(TCodeGen* cg, tNode* node) { 
    CodeGenExpr(cg, node->left);

    size_t printIntAddr = FindFunc(cg, keyPrintInt);
    if (!printIntAddr) {
        fprintf(stderr, "Function %s not found\n", keyPrintInt);
        exit(1);
    }

    mov_reg_reg(cg, REG_DI, REG_AX);

    push_reg(cg, REG_BX);
    push_reg(cg, REG_CX);
    push_reg(cg, REG_DX);

    sub_reg_imm32(cg, REG_SP, 8);
    call_rel32(cg, printIntAddr - (cg->size + 5));
    add_reg_imm32(cg, REG_SP, 8);

    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_BX);
}

static void CreatePrintAscii(TCodeGen* cg) {
    AddFunc(cg, keyPrintAscii); 

    push_reg(cg, REG_BP);
    mov_reg_reg(cg, REG_BP, REG_SP);
    sub_reg_imm32(cg, REG_SP, 8);

    push_reg(cg, REG_DI);

    mov_reg_imm32(cg, REG_AX, 1);       // number of syscall write 
    mov_reg_imm32(cg, REG_DI, 1);       // stdout
    mov_reg_reg(cg, REG_SI, REG_SP);    // pointer to value
    mov_reg_imm32(cg, REG_DX, 8);       // output size

    jge_rel32(cg, 0);

    syscall(cg);

    mov_reg_reg(cg, REG_SP, REG_BP);
    pop_reg(cg, REG_BP);
    ret(cg);
}

static void CreatePrintInt(TCodeGen* cg) {
    AddFunc(cg, keyPrintInt);

    size_t printAsciiAddr = FindFunc(cg, keyPrintAscii);
    if (!printAsciiAddr) {
        fprintf(stderr, "Function %s not found\n", keyPrintAscii);
        exit(1);
    }

    push_reg(cg, REG_BP);
    mov_reg_reg(cg, REG_BP, REG_SP);

    xor_reg_reg(cg, REG_CX, REG_CX);
    mov_reg_reg(cg, REG_AX, REG_DI);

    cmp_reg_imm32(cg, REG_AX, 0);
    int32_t jmpPos_1 = (int32_t)cg->size;
    jge_rel32(cg, 0);

    neg_reg(cg, REG_AX); 
    mov_reg_imm32(cg, REG_DI, '-');
                                   
    push_reg(cg, REG_AX);         
    push_reg(cg, REG_SI);          
    push_reg(cg, REG_DX); 
    push_reg(cg, REG_CX);         
    sub_reg_imm32(cg, REG_SP, 8);  
    call_rel32(cg, printAsciiAddr - (cg->size + 5));               
    add_reg_imm32(cg, REG_SP, 8); 
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_DX);          
    pop_reg(cg, REG_SI);          
    pop_reg(cg, REG_AX);
    
    int32_t jmpTarget_1 = (int32_t)cg->size;
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    memcpy(cg->code + jmpPos_1 + 2, (uint8_t*)&jmpOffset_1, 4);

    int32_t jmpTarget_2 = (int32_t)cg->size;

    xor_reg_reg(cg, REG_DX, REG_DX);
    mov_reg_imm32(cg, REG_BX, 10);
    idiv_reg(cg, REG_BX);
    push_reg(cg, REG_DX);
    inc_reg(cg, REG_CX);

    cmp_reg_imm32(cg, REG_AX, 0);
    int32_t jmpPos_2 = (int32_t)cg->size;
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 6);
    jne_rel32(cg, jmpOffset_2);

    int32_t jmpTarget_4 = cg->size;

    cmp_reg_imm32(cg, REG_CX, 0);
    int32_t jmpPos_3 = (int32_t)cg->size;
    je_rel32(cg, 0);

    pop_reg(cg, REG_DI);
    add_reg_imm32(cg, REG_DI, '0');

    push_reg(cg, REG_AX);         
    push_reg(cg, REG_SI);          
    push_reg(cg, REG_DX);  
    push_reg(cg, REG_CX);        
    sub_reg_imm32(cg, REG_SP, 8);  
    call_rel32(cg, printAsciiAddr - (cg->size + 5));               
    add_reg_imm32(cg, REG_SP, 8);
    pop_reg(cg, REG_CX); 
    pop_reg(cg, REG_DX);          
    pop_reg(cg, REG_SI);          
    pop_reg(cg, REG_AX);

    dec_reg(cg, REG_CX);

    int32_t jmpPos_4 = cg->size;
    int32_t jmpOffset_4 = jmpTarget_4 - (jmpPos_4 + 5);
    jmp_rel32(cg, jmpOffset_4); 

    int32_t jmpTarget_3 = (int32_t)cg->size;
    int32_t jmpOffset_3 = jmpTarget_3 - (jmpPos_3 + 6);
    memcpy(cg->code + jmpPos_3 + 2, (uint8_t*)&jmpOffset_3, 4);

    mov_reg_imm32(cg, REG_DI, '\n');
                                   
    push_reg(cg, REG_AX);         
    push_reg(cg, REG_SI);          
    push_reg(cg, REG_DX); 
    push_reg(cg, REG_CX);         
    sub_reg_imm32(cg, REG_SP, 8);  
    call_rel32(cg, printAsciiAddr - (cg->size + 5));               
    add_reg_imm32(cg, REG_SP, 8); 
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_DX);          
    pop_reg(cg, REG_SI);          
    pop_reg(cg, REG_AX);

    mov_reg_reg(cg, REG_SP, REG_BP);
    pop_reg(cg, REG_BP);
    ret(cg);
}