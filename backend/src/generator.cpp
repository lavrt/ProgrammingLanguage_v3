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

static void CodeGenExpr(CodeGen* cg, Node* node);
static void CodeGenStmt(CodeGen* cg, Node* node);

static void EmitNumber(CodeGen* cg, Node* node);
static void EmitIdentifier(CodeGen* cg, Node* node);
static void EmitCallInt(CodeGen* cg, Node* node);
static void EmitReadInt(CodeGen* cg);

static void EmitAdd(CodeGen* cg, Node* node);
static void EmitSub(CodeGen* cg, Node* node);
static void EmitMul(CodeGen* cg, Node* node);
static void EmitDiv(CodeGen* cg, Node* node);
static void EmitGreater(CodeGen* cg, Node* node);
static void EmitGreaterOrEqual(CodeGen* cg, Node* node);
static void EmitLess(CodeGen* cg, Node* node);
static void EmitLessOrEqual(CodeGen* cg, Node* node);
static void EmitIdentical(CodeGen* cg, Node* node);
static void EmitNotIdentical(CodeGen* cg, Node* node);

static void EmitDef(CodeGen* cg, Node* node);
static void EmitSemicolon(CodeGen* cg, Node* node);
static void EmitEqual(CodeGen* cg, Node* node);
static void EmitPrintAscii(CodeGen* cg, Node* node);
static void EmitPrintInt(CodeGen* cg, Node* node);
static void EmitIf(CodeGen* cg, Node* node);
static void EmitWhile(CodeGen* cg, Node* node);
static void EmitReturn(CodeGen* cg, Node* node);
static void EmitCallVoid(CodeGen* cg, Node* node); 

// global ------------------------------------------------------------------------------------------

void CodegenProgram(CodeGen* cg, Node* program, Elf64_Ehdr* ehdr) {
    CreateStandartFunctions(cg);

    CodeGenStmt(cg, program);

    size_t addr = cg->funcs.FindFunction(kEntryFunctionName);
    if (!addr) {
        std::cerr << "The function \"" << kEntryFunctionName << "\" was not found." << std::endl;
        exit(EXIT_FAILURE);
    }
    ehdr->e_entry += addr;
}

// static ------------------------------------------------------------------------------------------

static void CodeGenExpr(CodeGen* cg, Node* node) {
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

static void CodeGenStmt(CodeGen* cg, Node* node) {
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

static void EmitNumber(CodeGen* cg, Node* node) {
    x86_64::push(cg, std::stoi(node->GetValue()));
}

static void EmitIdentifier(CodeGen* cg, Node* node) {
    int offset = cg->vars.FindSymbol(node->GetValue());
    if (offset == -1) {
        std::cerr << "Undefined variable \"" << node->GetValue() << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }

    x86_64::mov(cg, x86_64::r64::rax, x86_64::r64::rbp, -offset);
    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitAdd(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);
    
    x86_64::add(cg, x86_64::r64::rax, x86_64::r64::rbx);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitSub(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::sub(cg, x86_64::r64::rax, x86_64::r64::rbx);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitMul(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::imul(cg, x86_64::r64::rax, x86_64::r64::rbx);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitDiv(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::mov(cg, x86_64::r64::rdx, 0);
    x86_64::idiv(cg, x86_64::r64::rbx);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitGreater(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::setg(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitGreaterOrEqual(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::setge(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitLess(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::setl(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitLessOrEqual(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::setle(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitIdentical(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::sete(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitNotIdentical(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rbx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, x86_64::r64::rbx);
    x86_64::setne(cg, x86_64::r64::rax);
    x86_64::movzx(cg, x86_64::r64::rax, x86_64::r8::al);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitCallVoid(CodeGen* cg, Node* node) {
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

    size_t addr = cg->funcs.FindFunction(node->GetValue());
    if (!addr) {
        std::cerr << "Undefined function \"" << node->GetValue() << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }
    x86_64::call(cg, (int32_t)(addr - (cg->code.GetSize() + 5)));

    x86_64::pop(cg, x86_64::r64::r9);
    x86_64::pop(cg, x86_64::r64::r8);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rsi);
    x86_64::pop(cg, x86_64::r64::rdi);
}

static void EmitCallInt(CodeGen* cg, Node* node) {
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
    
    size_t addr = cg->funcs.FindFunction(node->GetValue());
    if (!addr) {
        std::cerr << "Undefined function \"" << node->GetValue() << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }
    x86_64::call(cg, (int32_t)(addr - (cg->code.GetSize() + 5)));

    x86_64::pop(cg, x86_64::r64::r9);
    x86_64::pop(cg, x86_64::r64::r8);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rsi);
    x86_64::pop(cg, x86_64::r64::rdi);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitReadInt(CodeGen* cg) {
    size_t readIntAddr = cg->funcs.FindFunction(keyReadInt);
    if (!readIntAddr) {
        std::cerr << "Function \"" << keyReadInt << "\" is not found." << std::endl;
        exit(EXIT_FAILURE);
    }
  
    x86_64::push(cg, x86_64::r64::rcx);
    x86_64::push(cg, x86_64::r64::rdx);
    x86_64::push(cg, x86_64::r64::rdi);

    x86_64::call(cg, (int32_t)(readIntAddr - (cg->code.GetSize() + 5)));

    x86_64::pop(cg, x86_64::r64::rdi);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rcx);

    x86_64::push(cg, x86_64::r64::rax);
}

static void EmitDef(CodeGen* cg, Node* node) {
    cg->funcs.AddFunction(node->GetValue(), cg->code.GetSize());
    cg->vars.EnterScope();

    x86_64::push(cg, x86_64::r64::rbp);
    x86_64::mov(cg, x86_64::r64::rbp, x86_64::r64::rsp);

    Node* arg = node->GetLeft();
    int argCount = 0;
    while (arg && argCount < 6) {
        int offset = cg->vars.AddSymbol(arg->GetValue());
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

    cg->vars.ExitScope();
}

static void EmitSemicolon(CodeGen* cg, Node* node) {
    CodeGenStmt(cg, node->GetLeft());
    CodeGenStmt(cg, node->GetRight());
}

static void EmitEqual(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetRight());
    x86_64::pop(cg, x86_64::r64::rax);

    int offset = cg->vars.FindSymbol(node->GetLeft()->GetValue());
    if (offset == -1) {
        offset = cg->vars.AddSymbol(node->GetLeft()->GetValue());
        x86_64::sub(cg, x86_64::r64::rsp, 8);
    }

    x86_64::mov(cg, x86_64::r64::rbp, -offset, x86_64::r64::rax);
}

static void EmitPrintAscii(CodeGen* cg, Node* node) {
    size_t printAsciiAddr = cg->funcs.FindFunction(keyPrintAscii);
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

    x86_64::call(cg, (int32_t)(printAsciiAddr - (cg->code.GetSize() + 5)));

    x86_64::pop(cg, x86_64::r64::rdi);
    x86_64::pop(cg, x86_64::r64::rsi);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rax);
}

static void EmitPrintInt(CodeGen* cg, Node* node) { 
    size_t printIntAddr = cg->funcs.FindFunction(keyPrintInt);
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

    x86_64::call(cg, (int32_t)(printIntAddr - (cg->code.GetSize() + 5)));

    x86_64::pop(cg, x86_64::r64::rdi);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rax);
}

static void EmitIf(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    x86_64::pop(cg, x86_64::r64::rax);
    
    x86_64::cmp(cg, x86_64::r64::rax, 0);
    int32_t jmpPos_1 = (int32_t)cg->code.GetSize();
    x86_64::je(cg, 0);

    cg->vars.EnterScope();

    CodeGenStmt(cg, node->GetRight());

    cg->vars.ExitScope();

    int32_t jmpTarget_1 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    cg->code.InsertNumber(jmpOffset_1, jmpPos_1 + 2);
}

static void EmitWhile(CodeGen* cg, Node* node) {
    int32_t jmpTarget_2 = (int32_t)cg->code.GetSize();
    
    CodeGenExpr(cg, node->GetLeft());
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::cmp(cg, x86_64::r64::rax, 0);
    int32_t jmpPos_1 = (int32_t)cg->code.GetSize();
    x86_64::je(cg, 0);

    cg->vars.EnterScope();

    CodeGenStmt(cg, node->GetRight());

    cg->vars.ExitScope();
    
    int32_t jmpPos_2 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 5);
    x86_64::jmp(cg, jmpOffset_2);

    int32_t jmpTarget_1 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    cg->code.InsertNumber(jmpOffset_1, jmpPos_1 + 2);
}

static void EmitReturn(CodeGen* cg, Node* node) {
    CodeGenExpr(cg, node->GetLeft());
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::mov(cg, x86_64::r64::rsp, x86_64::r64::rbp);
    x86_64::pop(cg, x86_64::r64::rbp);

    x86_64::ret(cg);
}
