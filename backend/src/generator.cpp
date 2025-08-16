#include "generator.h"

#include <iostream>

#include "asmCommands.h"
#include "standardFunctions.h"

// static ------------------------------------------------------------------------------------------

static const std::string kEntryFunctionName = "main";
static const x86_64::r64 kArgRegs[] {
    x86_64::r64::rdi,
    x86_64::r64::rsi,
    x86_64::r64::rdx,
    x86_64::r64::rcx,
    x86_64::r64::r8,
    x86_64::r64::r9,
};

static int FindVar(TCodeGen* cg, const std::string& id);
static int AddVar(TCodeGen* cg, const std::string& id);
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
}

void AppendCode(TCodeGen* cg, std::span<uint8_t> data) {
    cg->code.insert(cg->code.end(), data.begin(), data.end());
}

void CodegenProgram(TCodeGen* cg, Node* program, Elf64_Ehdr* ehdr) {
    CreateStandartFunctions(cg);

    CodeGenStmt(cg, program);

    size_t addr = FindFunc(cg, kEntryFunctionName);
    if (!addr) {
        std::cerr << "The function \"" << kEntryFunctionName << "\" was not found." << std::endl;
        exit(EXIT_FAILURE);
    }
    ehdr->e_entry += addr;
}

void AddFunc(TCodeGen* cg, const std::string& name) {
    cg->funcs[name] = cg->code.size();
}

// static ------------------------------------------------------------------------------------------

static int FindVar(TCodeGen* cg, const std::string& id) {
    return cg->vars.Lookup(id);
}

static int AddVar(TCodeGen* cg, const std::string& id) {
    cg->stackOffset += 8;
    cg->vars.AddSymbol(id, cg->stackOffset);
    return cg->stackOffset;
}

static size_t FindFunc(const TCodeGen* const cg, const std::string& name) {
    auto it = cg->funcs.find(name);
    return it == cg->funcs.end() ? 0 : it->second;
}

static void CodeGenExpr(TCodeGen* cg, Node* node) {
    switch (node->GetType()) {
        case Number:            EmitNumber(cg, node);          break;
        case Identifier:        EmitIdentifier(cg, node);      break;      
        case Call:              EmitCallInt(cg, node);         break;
        case ReadInt:           EmitReadInt(cg);               break;     
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
        case Call:          EmitCallVoid(cg, node);        break;

        default: {
            std::cerr << "Unknown node type \"" << node->GetType() << "\"." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

static void EmitNumber(TCodeGen* cg, Node* node) {
    x86_64::push(cg, std::stoi(node->GetValue()));
}

static void EmitIdentifier(TCodeGen* cg, Node* node) {
    int offset = FindVar(cg, node->GetValue());
    if (offset == -1) {
        std::cerr << "Undefined variable \"" << node->GetValue() << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }

    x86_64::mov(cg, x86_64::r64::rax, x86_64::r64::rbp, -offset);
    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitAdd(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);
    
    x86_64::add(cg, x86_64::r64::rax, x86_64::r64::rbx);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitSub(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::sub(cg, x86_64::r64::rax, x86_64::r64::rbx);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitMul(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::imul(cg, x86_64::r64::rax, x86_64::r64::rbx);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitDiv(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::mov(cg, x86_64::r64::rdx, 0);
    x86_64::idiv(cg, x86_64::r64::rbx);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitGreater(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::setg(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitGreaterOrEqual(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::setge(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitLess(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::setl(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitLessOrEqual(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::setle(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitIdentical(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::sete(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitNotIdentical(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::setne(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitCallVoid(TCodeGen* cg, Node* node) {
    x86_64::push(cg, x86_64::r64::rdi);
    x86_64::push(cg, x86_64::r64::rsi);
    x86_64::push(cg, x86_64::r64::rdx);
    x86_64::push(cg, x86_64::r64::rcx);
    x86_64::push(cg, x86_64::r64::r8);
    x86_64::push(cg, x86_64::r64::r9);

    Node* arg = node->GetLeft();

    size_t argCount = 0;
    while (arg && argCount < 6) {
        CodeGenExpr(cg, arg);
        x86_64::pop(cg, kArgRegs[argCount++]);
        arg = arg->GetLeft();
    }

    size_t addr = FindFunc(cg, node->GetValue());
    if (!addr) {
        std::cerr << "Undefined function \"" << node->GetValue() << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }
    x86_64::call(cg, (int32_t)(addr - (cg->code.size() + 5)));

    x86_64::pop(cg, x86_64::r64::r9);
    x86_64::pop(cg, x86_64::r64::r8);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rsi);
    x86_64::pop(cg, x86_64::r64::rdi);
}

static void EmitCallInt(TCodeGen* cg, Node* node) {
    x86_64::push(cg, x86_64::r64::rdi);
    x86_64::push(cg, x86_64::r64::rsi);
    x86_64::push(cg, x86_64::r64::rdx);
    x86_64::push(cg, x86_64::r64::rcx);
    x86_64::push(cg, x86_64::r64::r8);
    x86_64::push(cg, x86_64::r64::r9);

    Node* arg = node->GetLeft();

    size_t argCount = 0;
    while (arg && argCount < 6) {
        CodeGenExpr(cg, arg);
        x86_64::pop(cg, kArgRegs[argCount++]);
        arg = arg->GetLeft();
    }
    
    size_t addr = FindFunc(cg, node->GetValue());
    if (!addr) {
        std::cerr << "Undefined function \"" << node->GetValue() << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }
    x86_64::call(cg, (int32_t)(addr - (cg->code.size() + 5)));

    x86_64::pop(cg, x86_64::r64::r9);
    x86_64::pop(cg, x86_64::r64::r8);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rsi);
    x86_64::pop(cg, x86_64::r64::rdi);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitReadInt(TCodeGen* cg) {
    size_t readIntAddr = FindFunc(cg, keyReadInt);
    if (!readIntAddr) {
        std::cerr << "Function \"" << keyReadInt << "\" is not found." << std::endl;
        exit(EXIT_FAILURE);
    }
  
    x86_64::push(cg, x86_64::r64::rcx);
    x86_64::push(cg, x86_64::r64::rdx);
    x86_64::push(cg, x86_64::r64::rdi);

    x86_64::call(cg, (int32_t)(readIntAddr - (cg->code.size() + 5)));

    x86_64::pop(cg, x86_64::r64::rdi);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rcx);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitDef(TCodeGen* cg, Node* node) {
    AddFunc(cg, node->GetValue());
    cg->vars.EnterScope();

    x86_64::push(cg, x86_64::r64::rbp);
    x86_64::mov(cg, x86_64::r64::rbp, x86_64::r64::rsp);

    Node* arg = node->GetLeft();
    int argCount = 0;
    while (arg && argCount < 6) {
        int offset = AddVar(cg, arg->GetValue());
        x86_64::mov(cg, x86_64::r64::rbp, -offset, kArgRegs[argCount]);
        arg = arg->GetLeft();
        ++argCount;
    }
    x86_64::sub(cg, x86_64::r64::rsp, 8 * argCount);

    CodeGenStmt(cg, node->GetRight());

    x86_64::mov(cg, x86_64::r64::rsp, x86_64::r64::rbp);
    x86_64::pop(cg, x86_64::r64::rbp);

    if (node->GetValue() == kEntryFunctionName) {
        x86_64::mov(cg, x86_64::r64::rax, 60);
        x86_64::mov(cg, x86_64::r64::rdi, 0);
        x86_64::syscall(cg);
    }

    x86_64::mov(cg, x86_64::r64::rax, -1);
    x86_64::ret(cg);

    cg->stackOffset = 0;

    cg->vars.ExitScope();
}

static void EmitSemicolon(TCodeGen* cg, Node* node) {
    CodeGenStmt(cg, node->GetLeft());
    CodeGenStmt(cg, node->GetRight());
}

static void EmitEqual(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rax);

    int offset = FindVar(cg, node->GetLeft()->GetValue());
    if (offset == -1) {
        offset = AddVar(cg, node->GetLeft()->GetValue());
        x86_64::sub(cg, x86_64::r64::rsp, 8);
    }

    x86_64::mov(cg, x86_64::r64::rbp, -offset, x86_64::r64::rax);
}

static void EmitPrintAscii(TCodeGen* cg, Node* node) {
    size_t printAsciiAddr = FindFunc(cg, keyPrintAscii);
    if (!printAsciiAddr) {
        std::cerr << "Function \"" << keyPrintAscii << "\" is not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    CodeGenExpr(cg, node->GetLeft());
    x86_64::pop(cg, x86_64::r64::rdi);  

    x86_64::push(cg, x86_64::r64::rax);
    x86_64::push(cg, x86_64::r64::rcx);
    x86_64::push(cg, x86_64::r64::rdx);
    x86_64::push(cg, x86_64::r64::rsi);
    x86_64::push(cg, x86_64::r64::rdi);

    x86_64::call(cg, (int32_t)(printAsciiAddr - (cg->code.size() + 5)));

    x86_64::pop(cg, x86_64::r64::rdi);
    x86_64::pop(cg, x86_64::r64::rsi);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rax);
}

static void EmitPrintInt(TCodeGen* cg, Node* node) { 
    size_t printIntAddr = FindFunc(cg, keyPrintInt);
    if (!printIntAddr) {
        std::cerr << "Function \"" << keyPrintInt << "\" is not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    CodeGenExpr(cg, node->GetLeft());
    x86_64::pop(cg, x86_64::r64::rdi);  

    x86_64::push(cg, x86_64::r64::rax);    
    x86_64::push(cg, x86_64::r64::rcx);
    x86_64::push(cg, x86_64::r64::rdx);
    x86_64::push(cg, x86_64::r64::rdi);

    x86_64::call(cg, (int32_t)(printIntAddr - (cg->code.size() + 5)));

    x86_64::pop(cg, x86_64::r64::rdi);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rax);
}

static void EmitIf(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    x86_64::pop(cg, x86_64::r64::rax);
    
    x86_64::cmp(cg, x86_64::r64::rax, 0);
    int32_t jmpPos_1 = (int32_t)cg->code.size();
    x86_64::je(cg, 0);

    CodeGenStmt(cg, node->GetRight());

    int32_t jmpTarget_1 = (int32_t)cg->code.size();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    std::copy((uint8_t*)&jmpOffset_1, (uint8_t*)&jmpOffset_1 + 4, cg->code.begin() + jmpPos_1 + 2);
}

static void EmitWhile(TCodeGen* cg, Node* node) {
    int32_t jmpTarget_2 = (int32_t)cg->code.size();
    
    CodeGenExpr(cg, node->GetLeft());
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, 0);
    int32_t jmpPos_1 = (int32_t)cg->code.size();
    x86_64::je(cg, 0);

    CodeGenStmt(cg, node->GetRight());
    
    int32_t jmpPos_2 = (int32_t)cg->code.size();
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 5);
    x86_64::jmp(cg, jmpOffset_2);

    int32_t jmpTarget_1 = (int32_t)cg->code.size();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    std::copy((uint8_t*)&jmpOffset_1, (uint8_t*)&jmpOffset_1 + 4, cg->code.begin() + jmpPos_1 + 2);
}

static void EmitReturn(TCodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::mov(cg, x86_64::r64::rsp, x86_64::r64::rbp);
    x86_64::pop(cg, x86_64::r64::rbp);

    x86_64::ret(cg);
}
