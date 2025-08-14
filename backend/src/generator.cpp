#include "generator.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <iostream>

#include "asmCommands.h"
#include "standardFunctions.h"

// static ------------------------------------------------------------------------------------------

static const std::string kEntryFunctionName = "main";
static const ERegister kArgRegs[] {REG_DI, REG_SI, REG_DX, REG_CX, REG_R8, REG_R9};

static int FindVar(const TCodeGen* const cg, const std::string& id);
static void AddVar(TCodeGen* cg, const std::string& id);
static size_t FindFunc(const TCodeGen* const cg, const std::string& name);
static void CodeGenExpr(TCodeGen* cg, Node* node);
static void CodeGenStmt(TCodeGen* cg, Node* node);


static void EmitNumber(TCodeGen* cg, Node* node);
static void EmitIdentifier(TCodeGen* cg, Node* node);
static void EmitCallInt(TCodeGen* cg, Node* node);
static void EmitReadInt(TCodeGen* cg);

static void EmitAdd(TCodeGen* cg, Node* node);
static void EmitSub(TCodeGen* cg, Node* node);
static void EmitMul(TCodeGen* cg, Node* node);
static void EmitDiv(TCodeGen* cg, Node* node);
static void EmitGreater(TCodeGen* cg, Node* node);
static void EmitGreaterOrEqual(TCodeGen* cg, Node* node);
static void EmitLess(TCodeGen* cg, Node* node);
static void EmitLessOrEqual(TCodeGen* cg, Node* node);
static void EmitIdentical(TCodeGen* cg, Node* node);
static void EmitNotIdentical(TCodeGen* cg, Node* node);

static void EmitDef(TCodeGen* cg, Node* node);
static void EmitSemicolon(TCodeGen* cg, Node* node);
static void EmitEqual(TCodeGen* cg, Node* node);
static void EmitPrintAscii(TCodeGen* cg, Node* node);
static void EmitPrintInt(TCodeGen* cg, Node* node);
static void EmitIf(TCodeGen* cg, Node* node);
static void EmitWhile(TCodeGen* cg, Node* node);
static void EmitReturn(TCodeGen* cg, Node* node);
static void EmitCallVoid(TCodeGen* cg, Node* node); 

// global ------------------------------------------------------------------------------------------

void CodeGenCtor(TCodeGen* cg) {
    cg->stackOffset = 0;
    cg->localStackOffset = 0;
    cg->isLocal = false;
}

void AppendCode(TCodeGen* cg, std::span<const uint8_t> data, size_t len) {
    cg->code.insert(cg->code.end(), data.begin(), data.end());
}

void CodegenProgram(TCodeGen* cg, Node* program, Elf64_Ehdr* ehdr) {
    CreateStandartFunctions(cg, ehdr);

    CodeGenStmt(cg, program);

    size_t addr = FindFunc(cg, kEntryFunctionName);
    if (!addr) {
        std::cerr << "The function \"" << kEntryFunctionName << "\" was not found." << std::endl;
        exit(EXIT_FAILURE);
    }
    call_rel32(cg, (int32_t)(addr - (cg->code.size() + 5)));
    
    mov_reg_imm32(cg, REG_AX, 60);
    mov_reg_imm32(cg, REG_DI, 0);
    syscall(cg);
}

void AddFunc(TCodeGen* cg, const std::string& name) {
    cg->funcs[name] = cg->code.size();
}

// static ------------------------------------------------------------------------------------------

static int FindVar(const TCodeGen* const cg, const std::string& id) {
    auto it = cg->vars.find(id);
    return it == cg->vars.end() ? -1 : it->second;
}

static void AddVar(TCodeGen* cg, const std::string& id) {
    int& varOffset = cg->isLocal ? cg->localStackOffset : cg->stackOffset;
    varOffset += 8;
    cg->vars[id] = varOffset;
}

static size_t FindFunc(const TCodeGen* const cg, const std::string& name) {
    auto it = cg->funcs.find(name);
    return it == cg->funcs.end() ? 0 : it->second;
}

static void CodeGenExpr(TCodeGen* cg, Node* node) {
    switch (node->GetType()) {
        case Number:            EmitNumber(cg, node);                  break;
        case Identifier:        EmitIdentifier(cg, node);              break;      
        case Call:              EmitCallInt(cg, node);                    break;
        case ReadInt:           EmitReadInt(cg);                       break;     
        case Add:               EmitAdd(cg, node);             break;
        case Sub:               EmitSub(cg, node);             break;
        case Mul:               EmitMul(cg, node);             break;
        case Div:               EmitDiv(cg, node);             break;
        case Greater:           EmitGreater(cg, node);         break;
        case GreaterOrEqual:    EmitGreaterOrEqual(cg, node);  break;
        case Less:              EmitLess(cg, node);            break;
        case LessOrEqual:       EmitLessOrEqual(cg, node);     break;
        case Identical:         EmitIdentical(cg, node);       break;
        case NotIdentical:      EmitNotIdentical(cg, node);    break;
        
        default: {
            std::cerr << "Unknown expression type \"" << node->GetType() << "\"." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

static void CodeGenStmt(TCodeGen* cg, Node* node) {
    switch (node->GetType()) {
        case End:                                          break;
        case Def:           EmitDef(cg, node);             break;
        case Semicolon:     EmitSemicolon(cg, node);       break;
        case Equal:         EmitEqual(cg, node);           break;
        case PrintAscii:    EmitPrintAscii(cg, node);      break;
        case PrintInt:      EmitPrintInt(cg, node);        break;
        case If:            EmitIf(cg, node);              break;
        case While:         EmitWhile(cg, node);           break;
        case Return:        EmitReturn(cg, node);          break;
        case Call:          EmitCallVoid(cg, node);            break;

        default: {
            std::cerr << "Unknown node type \"" << node->GetType() << "\"." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

static void EmitNumber(TCodeGen* cg, Node* node) {
    push_imm32(cg, std::stoi(node->GetValue()));
}

static void EmitIdentifier(TCodeGen* cg, Node* node) {
    int offset = FindVar(cg, node->GetValue());
    if (offset == -1) {
        std::cerr << "Undefined variable \"" << node->GetValue() << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }

    mov_reg_mem(cg, REG_AX, -offset);
    push_reg(cg, REG_AX);
}

static void EmitAdd(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_BX);
    pop_reg(cg, REG_AX);
    
    add_reg_reg(cg, REG_AX, REG_BX);

    push_reg(cg, REG_AX);
}

static void EmitSub(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_BX);
    pop_reg(cg, REG_AX);

    sub_reg_reg(cg, REG_AX, REG_BX);

    push_reg(cg, REG_AX);
}

static void EmitMul(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_BX);
    pop_reg(cg, REG_AX);

    imul_reg_reg(cg, REG_AX, REG_BX);

    push_reg(cg, REG_AX);
}

static void EmitDiv(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_BX);
    pop_reg(cg, REG_AX);

    xor_reg_reg(cg, REG_DX, REG_DX);
    idiv_reg(cg, REG_BX);

    push_reg(cg, REG_AX);
}

static void EmitGreater(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_BX);
    pop_reg(cg, REG_AX);

    cmp_reg_reg(cg, REG_AX, REG_BX);
    setg_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);

    push_reg(cg, REG_AX);
}

static void EmitGreaterOrEqual(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_BX);
    pop_reg(cg, REG_AX);

    cmp_reg_reg(cg, REG_AX, REG_BX);
    setge_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);

    push_reg(cg, REG_AX);
}

static void EmitLess(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_BX);
    pop_reg(cg, REG_AX);

    cmp_reg_reg(cg, REG_AX, REG_BX);
    setl_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);

    push_reg(cg, REG_AX);
}

static void EmitLessOrEqual(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_BX);
    pop_reg(cg, REG_AX);

    cmp_reg_reg(cg, REG_AX, REG_BX);
    setle_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);

    push_reg(cg, REG_AX);
}

static void EmitIdentical(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_BX);
    pop_reg(cg, REG_AX);

    cmp_reg_reg(cg, REG_AX, REG_BX);
    sete_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);

    push_reg(cg, REG_AX);
}

static void EmitNotIdentical(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_BX);
    pop_reg(cg, REG_AX);

    cmp_reg_reg(cg, REG_AX, REG_BX);
    setne_reg(cg, REG_AX);
    movzx_reg_reg(cg, REG_AX, REG_AX);

    push_reg(cg, REG_AX);
}

static void EmitCallVoid(TCodeGen* cg, Node* node) {
    push_reg(cg, REG_DI);
    push_reg(cg, REG_SI);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_CX);
    push_reg(cg, REG_R8);
    push_reg(cg, REG_R9);

    Node* arg = node->GetLeft();

    size_t argCount = 0;
    while (arg && argCount < 6) {
        CodeGenExpr(cg, arg);
        pop_reg(cg, kArgRegs[argCount++]);
        arg = arg->GetLeft();
    }

    size_t addr = FindFunc(cg, node->GetValue());
    if (!addr) {
        fprintf(stderr, "Undefined function\n");
        exit(EXIT_FAILURE);
    }
    call_rel32(cg, (int32_t)(addr - (cg->code.size() + 5)));

    pop_reg(cg, REG_R9);
    pop_reg(cg, REG_R8);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_SI);
    pop_reg(cg, REG_DI);
}

static void EmitCallInt(TCodeGen* cg, Node* node) {
    push_reg(cg, REG_DI);
    push_reg(cg, REG_SI);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_CX);
    push_reg(cg, REG_R8);
    push_reg(cg, REG_R9);

    Node* arg = node->GetLeft();

    size_t argCount = 0;
    while (arg && argCount < 6) {
        CodeGenExpr(cg, arg);
        pop_reg(cg, kArgRegs[argCount++]);
        arg = arg->GetLeft();
    }
    size_t addr = FindFunc(cg, node->GetValue());
    if (!addr) {
        fprintf(stderr, "Undefined function\n");
        exit(EXIT_FAILURE);
    }
    call_rel32(cg, (int32_t)(addr - (cg->code.size() + 5)));

    pop_reg(cg, REG_R9);
    pop_reg(cg, REG_R8);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_SI);
    pop_reg(cg, REG_DI);

    push_reg(cg, REG_AX);
}

static void EmitReadInt(TCodeGen* cg) {
    size_t readIntAddr = FindFunc(cg, keyReadInt);
    if (!readIntAddr) {
        std::cerr << "Function \"" << keyReadInt << "\" is not found." << std::endl;
        exit(EXIT_FAILURE);
    }
  
    push_reg(cg, REG_CX);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_DI);

    call_rel32(cg, (int32_t)(readIntAddr - (cg->code.size() + 5)));

    pop_reg(cg, REG_DI);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_CX);

    push_reg(cg, REG_AX);
}

static void EmitDef(TCodeGen* cg, Node* node) {
    cg->isLocal = true;
    int varCountBeforeFunction = cg->varCount;

    int32_t jmpPos1 = (int32_t)cg->code.size();
    jmp_rel32(cg, 0);

    AddFunc(cg, node->GetValue());

    push_reg(cg, REG_BP);
    mov_reg_reg(cg, REG_BP, REG_SP);

    Node* arg = node->GetLeft();
    int argCount = 0;
    while (arg && argCount < 6) {
        AddVar(cg, arg->GetValue());
        int offset = cg->vars[arg->GetValue()];
        mov_mem_reg(cg, -offset, kArgRegs[argCount]);
        arg = arg->GetLeft();
        ++argCount;
    }
    sub_reg_imm32(cg, REG_SP, 8 * argCount);

    CodeGenStmt(cg, node->GetRight());

    mov_reg_reg(cg, REG_SP, REG_BP);
    pop_reg(cg, REG_BP);

    mov_reg_imm32(cg, REG_AX, -1);
    ret(cg);

    int32_t jmpTarget1 = (int32_t)cg->code.size();
    int32_t jmpOffset1 = jmpTarget1 - (jmpPos1 + 5);
    std::copy((uint8_t*)&jmpOffset1, (uint8_t*)&jmpOffset1 + 4, cg->code.begin() + jmpPos1 + 1);

    cg->isLocal = false;
    cg->localStackOffset = 0;

    cg->varCount = varCountBeforeFunction;
}

static void EmitSemicolon(TCodeGen* cg, Node* node) {
    CodeGenStmt(cg, node->GetLeft());
    CodeGenStmt(cg, node->GetRight());
}

static void EmitEqual(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetRight());
    pop_reg(cg, REG_AX);

    int offset = FindVar(cg, node->GetLeft()->GetValue());
    if (offset == -1) {
        AddVar(cg, node->GetLeft()->GetValue());
        offset = cg->vars[node->GetLeft()->GetValue()];
        sub_reg_imm32(cg, REG_SP, 8);
    }

    mov_mem_reg(cg, -offset, REG_AX);
}

static void EmitPrintAscii(TCodeGen* cg, Node* node) {
    size_t printAsciiAddr = FindFunc(cg, keyPrintAscii);
    if (!printAsciiAddr) {
        std::cerr << "Function \"" << keyPrintAscii << "\" is not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    CodeGenExpr(cg, node->GetLeft());
    pop_reg(cg, REG_DI);  

    push_reg(cg, REG_AX);
    push_reg(cg, REG_CX);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_SI);
    push_reg(cg, REG_DI);

    call_rel32(cg, (int32_t)(printAsciiAddr - (cg->code.size() + 5)));

    pop_reg(cg, REG_DI);
    pop_reg(cg, REG_SI);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_AX);
}

static void EmitPrintInt(TCodeGen* cg, Node* node) { 
    size_t printIntAddr = FindFunc(cg, keyPrintInt);
    if (!printIntAddr) {
        std::cerr << "Function \"" << keyPrintInt << "\" is not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    CodeGenExpr(cg, node->GetLeft());
    pop_reg(cg, REG_DI);  

    push_reg(cg, REG_AX);    
    push_reg(cg, REG_CX);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_DI);

    call_rel32(cg, (int32_t)(printIntAddr - (cg->code.size() + 5)));

    pop_reg(cg, REG_DI);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_AX);
}

static void EmitIf(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    pop_reg(cg, REG_AX);
    
    cmp_reg_imm32(cg, REG_AX, 0);
    int32_t jmpPos_1 = (int32_t)cg->code.size();
    je_rel32(cg, 0);

    CodeGenStmt(cg, node->GetRight());

    int32_t jmpTarget_1 = (int32_t)cg->code.size();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    std::copy((uint8_t*)&jmpOffset_1, (uint8_t*)&jmpOffset_1 + 4, cg->code.begin() + jmpPos_1 + 2);
}

static void EmitWhile(TCodeGen* cg, Node* node) {
    int32_t jmpTarget_2 = (int32_t)cg->code.size();
    
    CodeGenExpr(cg, node->GetLeft());
    pop_reg(cg, REG_AX);

    cmp_reg_imm32(cg, REG_AX, 0);
    int32_t jmpPos_1 = (int32_t)cg->code.size();
    je_rel32(cg, 0);

    CodeGenStmt(cg, node->GetRight());
    
    int32_t jmpPos_2 = (int32_t)cg->code.size();
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 5);
    jmp_rel32(cg, jmpOffset_2);

    int32_t jmpTarget_1 = (int32_t)cg->code.size();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    std::copy((uint8_t*)&jmpOffset_1, (uint8_t*)&jmpOffset_1 + 4, cg->code.begin() + jmpPos_1 + 2);
}

static void EmitReturn(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    pop_reg(cg, REG_AX);

    mov_reg_reg(cg, REG_SP, REG_BP);
    pop_reg(cg, REG_BP);

    ret(cg);
}
