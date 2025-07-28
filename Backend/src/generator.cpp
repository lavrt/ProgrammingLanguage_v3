#include "generator.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "asmCommands.h"
#include "standartFunctions.h"

// static ------------------------------------------------------------------------------------------

static const size_t kInitCapacityOfCodeArray = 4096;
static const int kAdditionalCapacityOfNameTable = 16;
static const char* const kNameOfEntryFunction = "_start";
static const ERegister kArgRegs[] = { REG_DI, REG_SI, REG_DX, REG_CX, REG_R8, REG_R9 };

static Operations GetOperationType(const char* const word);
static int FindVar(const TCodeGen* const cg, const char* id);
static void AddVar(TCodeGen* cg, const char* id);
static size_t FindFunc(const TCodeGen* const cg, const char* name);
static void CodeGenExpr(TCodeGen* cg, tNode* node);
static void CodeGenStmt(TCodeGen* cg, tNode* node);

namespace GenExpr {
    static void EmitNumber(TCodeGen* cg, tNode* node);

    static void EmitIdentifier(TCodeGen* cg, tNode* node);

    static void EmitCalling(TCodeGen* cg, tNode* node);

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

    namespace Operation {
        static void EmitSemicolon(TCodeGen* cg, tNode* node);
        static void EmitEqual(TCodeGen* cg, tNode* node);
        static void EmitPrintAscii(TCodeGen* cg, tNode* node);
        static void EmitPrintInt(TCodeGen* cg, tNode* node);
        static void EmitIf(TCodeGen* cg, tNode* node);
        static void EmitWhile(TCodeGen* cg, tNode* node);
        static void EmitReturn(TCodeGen* cg, tNode* node);
    }
}

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

    cg->localStackOffset = 0;
    cg->isLocal = false;
}

void CodeGenDtor(TCodeGen* cg) {
    free(cg->code);
    free(cg->vars);
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
    CreateStandartFunctions(cg, ehdr);
    
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

void AddFunc(TCodeGen* cg, const char* name) {
    if (cg->funcCount >= kAdditionalCapacityOfNameTable) {
        cg->funcs = (TFunctions*)realloc(
            cg->funcs, ((long unsigned)(cg->funcCount + kAdditionalCapacityOfNameTable)) * sizeof(TFunctions)
        );
        assert(cg->funcs);
    }
    cg->funcs[cg->funcCount].name = name;
    cg->funcs[cg->funcCount].addr = cg->size;
    ++cg->funcCount;
}

// static ------------------------------------------------------------------------------------------

static Operations GetOperationType(const char* const word) {
         if (!strcmp(word, keyIf)) return If;
    else if (!strcmp(word, keyAdd)) return Add;
    else if (!strcmp(word, keySub)) return Sub;
    else if (!strcmp(word, keyMul)) return Mul;
    else if (!strcmp(word, keyDiv)) return Div;
    else if (!strcmp(word, keyWhile)) return While; 
    else if (!strcmp(word, keyEqual)) return Equal; 
    else if (!strcmp(word, keyPrintAscii)) return PrintAscii;
    else if (!strcmp(word, keyPrintInt)) return PrintInt;
    else if (!strcmp(word, keyReturn)) return Return;
    else if (!strcmp(word, keyGreater)) return Greater;
    else if (!strcmp(word, keyLess)) return Less;
    else if (!strcmp(word, keySemicolon)) return Semicolon;
    else if (!strcmp(word, keyIdentical)) return Identical;
    else if (!strcmp(word, keyLessOrEqual)) return LessOrEqual;
    else if (!strcmp(word, keyNotIdentical)) return NotIdentical;
    else if (!strcmp(word, keyGreaterOrEqual)) return GreaterOrEqual;

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
    int& varOffset = cg->isLocal ? cg->localStackOffset : cg->stackOffset;
    if (cg->varCount >= kAdditionalCapacityOfNameTable) {
        cg->vars = (TVariables*)realloc(
            cg->vars, ((long unsigned)(cg->varCount + kAdditionalCapacityOfNameTable)) * sizeof(TVariables)
        );
        assert(cg->vars);
    }
    cg->vars[cg->varCount].id = id;
    varOffset += 8;
    cg->vars[cg->varCount].offset = varOffset;
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

static void CodeGenExpr(TCodeGen* cg, tNode* node) {
    switch (node->type) {
        case Number:        GenExpr::EmitNumber(cg, node);      break;
        case Identifier:    GenExpr::EmitIdentifier(cg, node);  break;      
        case Calling:       GenExpr::EmitCalling(cg, node);     break;
        case Binary: {
            CodeGenExpr(cg, node->left);
            CodeGenExpr(cg, node->right);
            pop_reg(cg, REG_BX);
            pop_reg(cg, REG_AX);
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
                    exit(EXIT_FAILURE);
                }
            }
            push_reg(cg, REG_AX);
        } break;
        
        default: {
            fprintf(stderr, "Unknown expression type\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void CodeGenStmt(TCodeGen* cg, tNode* node) {
    switch (node->type) {
        case Function:      GenStmt::EmitFunction(cg, node);        break;
        case Operation: {
            switch (GetOperationType(node->value)) {
                case Semicolon:     GenStmt::Operation::EmitSemicolon(cg, node);        break;
                case Equal:         GenStmt::Operation::EmitEqual(cg, node);            break;
                case PrintAscii:    GenStmt::Operation::EmitPrintAscii(cg, node);       break;
                case PrintInt:      GenStmt::Operation::EmitPrintInt(cg, node);         break;
                case If:            GenStmt::Operation::EmitIf(cg, node);               break;
                case While:         GenStmt::Operation::EmitWhile(cg, node);            break;
                case Return:        GenStmt::Operation::EmitReturn(cg, node);           break;

                default: {
                    fprintf(stderr, "Unknown operation \"%s\"\n", node->value);
                    exit(EXIT_FAILURE);
                }
            }
        } break;

        default: {
            fprintf(stderr, "Unknown node type\n");
            exit(EXIT_FAILURE);
        }
    }
}

static void GenExpr::EmitNumber(TCodeGen* cg, tNode* node) {
    push_imm32(cg, atoi(node->value));
}

static void GenExpr::EmitIdentifier(TCodeGen* cg, tNode* node) {
    int offset = FindVar(cg, node->value);
    if (offset == -1) {
        fprintf(stderr, "Undefined variable\n");
        exit(EXIT_FAILURE);
    }

    mov_reg_mem(cg, REG_AX, -offset);
    push_reg(cg, REG_AX);
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
    push_reg(cg, REG_DX);
    push_reg(cg, REG_CX);
    push_reg(cg, REG_R8);
    push_reg(cg, REG_R9);

    tNode* arg = node->left;

    size_t argCount = 0;
    while (arg && argCount < 6) {
        CodeGenExpr(cg, arg);
        pop_reg(cg, kArgRegs[argCount++]);
        arg = arg->left;
    }

    size_t addr = FindFunc(cg, node->value);
    if (!addr) {
        fprintf(stderr, "Undefined function\n");
        exit(EXIT_FAILURE);
    }
    call_rel32(cg, (int32_t)(addr - (cg->size + 5)));

    pop_reg(cg, REG_R9);
    pop_reg(cg, REG_R8);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_SI);
    pop_reg(cg, REG_DI);

    push_reg(cg, REG_AX);
}

static void GenStmt::EmitFunction(TCodeGen* cg, tNode* node) {
    cg->isLocal = true;
    int varCountBeforeFunction = cg->varCount;

    int32_t jmpPos1 = (int32_t)cg->size;
    jmp_rel32(cg, 0);

    AddFunc(cg, node->value);

    push_reg(cg, REG_BP);
    mov_reg_reg(cg, REG_BP, REG_SP);

    tNode* arg = node->left;
    int argCount = 0;
    while (arg && argCount < 6) {
        AddVar(cg, arg->value);
        int offset = cg->vars[cg->varCount - 1].offset;
        mov_mem_reg(cg, -offset, kArgRegs[argCount]);
        arg = arg->left;
        ++argCount;
    }
    sub_reg_imm32(cg, REG_SP, 8 * argCount);

    CodeGenStmt(cg, node->right);

    mov_reg_reg(cg, REG_SP, REG_BP);
    pop_reg(cg, REG_BP);

    mov_reg_imm32(cg, REG_AX, -1);
    ret(cg);

    int32_t jmpTarget1 = (int32_t)cg->size;
    int32_t jmpOffset1 = jmpTarget1 - (jmpPos1 + 5);
    memcpy(cg->code + jmpPos1 + 1, (uint8_t*)&jmpOffset1, 4);

    cg->isLocal = false;
    cg->localStackOffset = 0;

    cg->varCount = varCountBeforeFunction;
}

static void GenStmt::Operation::EmitSemicolon(TCodeGen* cg, tNode* node) {
    CodeGenStmt(cg, node->left);
    CodeGenStmt(cg, node->right);
}

static void GenStmt::Operation::EmitEqual(TCodeGen* cg, tNode* node) {
    CodeGenExpr(cg, node->right);
    pop_reg(cg, REG_AX);

    int offset = FindVar(cg, node->left->value);
    if (offset == -1) {
        AddVar(cg, node->left->value);
        offset = cg->vars[cg->varCount - 1].offset;
        sub_reg_imm32(cg, REG_SP, 8);
    }

    mov_mem_reg(cg, -offset, REG_AX);
}

static void GenStmt::Operation::EmitPrintAscii(TCodeGen* cg, tNode* node) {
    size_t printAsciiAddr = FindFunc(cg, keyPrintAscii);
    if (!printAsciiAddr) {
        fprintf(stderr, "Function %s not found\n", keyPrintAscii);
        exit(EXIT_FAILURE);
    }

    CodeGenExpr(cg, node->left);
    pop_reg(cg, REG_DI);  

    push_reg(cg, REG_AX);
    push_reg(cg, REG_CX);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_SI);
    push_reg(cg, REG_DI);

    call_rel32(cg, (int32_t)(printAsciiAddr - (cg->size + 5)));

    pop_reg(cg, REG_DI);
    pop_reg(cg, REG_SI);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_AX);
}

static void GenStmt::Operation::EmitPrintInt(TCodeGen* cg, tNode* node) { 
    size_t printIntAddr = FindFunc(cg, keyPrintInt);
    if (!printIntAddr) {
        fprintf(stderr, "Function %s not found\n", keyPrintInt);
        exit(EXIT_FAILURE);
    }

    CodeGenExpr(cg, node->left);
    pop_reg(cg, REG_DI);  

    push_reg(cg, REG_AX);    
    push_reg(cg, REG_CX);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_DI);

    call_rel32(cg, (int32_t)(printIntAddr - (cg->size + 5)));

    pop_reg(cg, REG_DI);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_AX);
}

static void GenStmt::Operation::EmitIf(TCodeGen* cg, tNode* node) {
    CodeGenExpr(cg, node->left);
    pop_reg(cg, REG_AX);
    
    cmp_reg_imm32(cg, REG_AX, 0);
    int32_t jmpPos_1 = (int32_t)cg->size;
    je_rel32(cg, 0);

    CodeGenStmt(cg, node->right);

    int32_t jmpTarget_1 = (int32_t)cg->size;
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    memcpy(cg->code + jmpPos_1 + 2, (uint8_t*)&jmpOffset_1, 4);
}

static void GenStmt::Operation::EmitWhile(TCodeGen* cg, tNode* node) {
    CodeGenExpr(cg, node->left);
    pop_reg(cg, REG_AX);

    int32_t jmpTarget_2 = (int32_t)cg->size;

    cmp_reg_imm32(cg, REG_AX, 0);
    int32_t jmpPos_1 = (int32_t)cg->size;
    je_rel32(cg, 0);

    CodeGenStmt(cg, node->right);
    
    int32_t jmpPos_2 = (int32_t)cg->size;
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 5);
    jmp_rel32(cg, jmpOffset_2);

    int32_t jmpTarget_1 = (int32_t)cg->size;
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    memcpy(cg->code + jmpPos_1 + 2, (uint8_t*)&jmpOffset_1, 4);
}

static void GenStmt::Operation::EmitReturn(TCodeGen* cg, tNode* node) {
    CodeGenExpr(cg, node->left);
    pop_reg(cg, REG_AX);

    mov_reg_reg(cg, REG_SP, REG_BP);
    pop_reg(cg, REG_BP);

    ret(cg);
}
