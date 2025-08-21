#include "generator.h"

#include <iostream>
#include <fstream>

#include "asmCommands.h"

// static ------------------------------------------------------------------------------------------

static const std::string kEntryFunctionName = "main";
static const r64 kArgRegs[] {
    r64::rdi,
    r64::rsi,
    r64::rdx,
    r64::rcx,
    r64::r8,
    r64::r9,
};

// -------------------------------------------------------------------------------------------------

void CodeGen::GenerateProgram(Node* program, const std::string& fileName) {
    CreateStandartFunctions();
    CodeGenStmt(program);

    size_t addr = funcs.FindFunction(kEntryFunctionName);
    if (!addr) {
        std::cerr << "The function \"" << kEntryFunctionName << "\" was not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    Elf64_Ehdr ehdr;
    CreateElfHeader(&ehdr);
    ehdr.e_entry += addr;

    Elf64_Phdr phdr;
    CreateProgramHeader(&phdr, asmGen.GetCodeSize());

    std::ofstream file(fileName, std::ios::binary); 
    if (!file) {
        std::cerr << "The \"" << fileName << "\" file cannot be opened. \n" <<
            "It is possible that this file is already being used by another process. "
            "Try to kill this process or delete the file." << std::endl;
        exit(EXIT_FAILURE);
    }

    file.write(reinterpret_cast<const char*>(&ehdr), kElfHeaderSize);
    file.write(reinterpret_cast<const char*>(&phdr), kProgramHeaderSize);
    file.write(reinterpret_cast<const char*>(asmGen.GetCodeData()), asmGen.GetCodeSize() * sizeof(uint8_t));

    if (!file.good()) {
        std::cerr << "Error writing to the \"" << fileName << "\" file." << std::endl;
        exit(EXIT_FAILURE);
    }

    file.close();

    std::cout << "Executable file created\n";
}

void CodeGen::CodeGenExpr(Node* node) {
    switch (node->GetType()) {
        case Number:            EmitNumber(node);          break;
        case Identifier:        EmitIdentifier(node);      break;      
        case Call:              EmitCallInt(node);         break;
        case ReadInt:           EmitReadInt();             break;     
        case Add:               EmitAdd(node);             break;
        case Sub:               EmitSub(node);             break;
        case Mul:               EmitMul(node);             break;
        case Div:               EmitDiv(node);             break;
        case Greater:           EmitGreater(node);         break;
        case GreaterOrEqual:    EmitGreaterOrEqual(node);  break;
        case Less:              EmitLess(node);            break;
        case LessOrEqual:       EmitLessOrEqual(node);     break;
        case Identical:         EmitIdentical(node);       break;
        case NotIdentical:      EmitNotIdentical(node);    break;
        
        default: {
            std::cerr << "Unknown expression type \"" << node->GetType() << "\"." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

void CodeGen::CodeGenStmt(Node* node) {
    switch (node->GetType()) {
        case End:                                      break;
        case Def:           EmitDef(node);             break;
        case Semicolon:     EmitSemicolon(node);       break;
        case Equal:         EmitEqual(node);           break;
        case PrintAscii:    EmitPrintAscii(node);      break;
        case PrintInt:      EmitPrintInt(node);        break;
        case If:            EmitIf(node);              break;
        case While:         EmitWhile(node);           break;
        case Return:        EmitReturn(node);          break;
        case Call:          EmitCallVoid(node);        break;

        default: {
            std::cerr << "Unknown node type \"" << node->GetType() << "\"." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

void CodeGen::EmitNumber(Node* node) {
    asmGen.push(std::stoi(node->GetValue()));
}

void CodeGen::EmitIdentifier(Node* node) {
    int offset = vars.FindSymbol(node->GetValue());
    if (offset == -1) {
        std::cerr << "Undefined variable \"" << node->GetValue() << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }

    asmGen.mov(r64::rax, r64::rbp, -offset);
    asmGen.push(r64::rax);
}

void CodeGen::EmitAdd(Node* node) {
    CodeGenExpr(node->GetLeft());
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rbx);
    asmGen.pop(r64::rax);

    asmGen.add(r64::rax, r64::rbx);

    asmGen.push(r64::rax);
}

void CodeGen::EmitSub(Node* node) {
    CodeGenExpr(node->GetLeft());
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rbx);
    asmGen.pop(r64::rax);

    asmGen.sub(r64::rax, r64::rbx);

    asmGen.push(r64::rax);
}

void CodeGen::EmitMul(Node* node) {
    CodeGenExpr(node->GetLeft());
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rbx);
    asmGen.pop(r64::rax);

    asmGen.imul(r64::rax, r64::rbx);

    asmGen.push(r64::rax);
}

void CodeGen::EmitDiv(Node* node) {
    CodeGenExpr(node->GetLeft());
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rbx);
    asmGen.pop(r64::rax);

    asmGen.mov(r64::rdx, 0);
    asmGen.idiv(r64::rbx);

    asmGen.push(r64::rax);
}

void CodeGen::EmitGreater(Node* node) {
    CodeGenExpr(node->GetLeft());
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rbx);
    asmGen.pop(r64::rax);

    asmGen.cmp(r64::rax, r64::rbx);
    asmGen.setg(r64::rax);
    asmGen.movzx(r64::rax, r8::al);

    asmGen.push(r64::rax);
}

void CodeGen::EmitGreaterOrEqual(Node* node) {
    CodeGenExpr(node->GetLeft());
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rbx);
    asmGen.pop(r64::rax);

    asmGen.cmp(r64::rax, r64::rbx);
    asmGen.setge(r64::rax);
    asmGen.movzx(r64::rax, r8::al);

    asmGen.push(r64::rax);
}

void CodeGen::EmitLess(Node* node) {
    CodeGenExpr(node->GetLeft());
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rbx);
    asmGen.pop(r64::rax);

    asmGen.cmp(r64::rax, r64::rbx);
    asmGen.setl(r64::rax);
    asmGen.movzx(r64::rax, r8::al);

    asmGen.push(r64::rax);
}

void CodeGen::EmitLessOrEqual(Node* node) {
    CodeGenExpr(node->GetLeft());
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rbx);
    asmGen.pop(r64::rax);

    asmGen.cmp(r64::rax, r64::rbx);
    asmGen.setle(r64::rax);
    asmGen.movzx(r64::rax, r8::al);

    asmGen.push(r64::rax);
}

void CodeGen::EmitIdentical(Node* node) {
    CodeGenExpr(node->GetLeft());
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rbx);
    asmGen.pop(r64::rax);

    asmGen.cmp(r64::rax, r64::rbx);
    asmGen.sete(r64::rax);
    asmGen.movzx(r64::rax, r8::al);

    asmGen.push(r64::rax);
}

void CodeGen::EmitNotIdentical(Node* node) {
    CodeGenExpr(node->GetLeft());
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rbx);
    asmGen.pop(r64::rax);

    asmGen.cmp(r64::rax, r64::rbx);
    asmGen.setne(r64::rax);
    asmGen.movzx(r64::rax, r8::al);

    asmGen.push(r64::rax);
}

void CodeGen::EmitCallVoid(Node* node) {
    asmGen.push(r64::rdi);
    asmGen.push(r64::rsi);
    asmGen.push(r64::rdx);
    asmGen.push(r64::rcx);
    asmGen.push(r64::r8);
    asmGen.push(r64::r9);

    Node* arg = node->GetLeft();

    size_t argCount = 0;
    while (arg && argCount < 6) {
        CodeGenExpr(arg);
        asmGen.pop(kArgRegs[argCount++]);
        arg = arg->GetLeft();
    }

    size_t addr = funcs.FindFunction(node->GetValue());
    if (!addr) {
        std::cerr << "Undefined function \"" << node->GetValue() << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }
    asmGen.call((int32_t)(addr - (asmGen.GetCodeSize() + 5)));

    asmGen.pop(r64::r9);
    asmGen.pop(r64::r8);
    asmGen.pop(r64::rcx);
    asmGen.pop(r64::rdx);
    asmGen.pop(r64::rsi);
    asmGen.pop(r64::rdi);
}

void CodeGen::EmitCallInt(Node* node) {
    asmGen.push(r64::rdi);
    asmGen.push(r64::rsi);
    asmGen.push(r64::rdx);
    asmGen.push(r64::rcx);
    asmGen.push(r64::r8);
    asmGen.push(r64::r9);

    Node* arg = node->GetLeft();

    size_t argCount = 0;
    while (arg && argCount < 6) {
        CodeGenExpr(arg);
        asmGen.pop(kArgRegs[argCount++]);
        arg = arg->GetLeft();
    }
    
    size_t addr = funcs.FindFunction(node->GetValue());
    if (!addr) {
        std::cerr << "Undefined function \"" << node->GetValue() << "\"." << std::endl;
        exit(EXIT_FAILURE);
    }
    asmGen.call((int32_t)(addr - (asmGen.GetCodeSize() + 5)));

    asmGen.pop(r64::r9);
    asmGen.pop(r64::r8);
    asmGen.pop(r64::rcx);
    asmGen.pop(r64::rdx);
    asmGen.pop(r64::rsi);
    asmGen.pop(r64::rdi);

    asmGen.push(r64::rax);
}

void CodeGen::EmitReadInt() {
    size_t readIntAddr = funcs.FindFunction(keyReadInt);
    if (!readIntAddr) {
        std::cerr << "Function \"" << keyReadInt << "\" is not found." << std::endl;
        exit(EXIT_FAILURE);
    }
  
    asmGen.push(r64::rcx);
    asmGen.push(r64::rdx);
    asmGen.push(r64::rdi);

    asmGen.call((int32_t)(readIntAddr - (asmGen.GetCodeSize() + 5)));

    asmGen.pop(r64::rdi);
    asmGen.pop(r64::rdx);
    asmGen.pop(r64::rcx);

    asmGen.push(r64::rax);
}

void CodeGen::EmitDef(Node* node) {
    funcs.AddFunction(node->GetValue(), asmGen.GetCodeSize());
    vars.EnterScope();

    asmGen.push(r64::rbp);
    asmGen.mov(r64::rbp, r64::rsp);

    Node* arg = node->GetLeft();
    int argCount = 0;
    while (arg && argCount < 6) {
        int offset = vars.AddSymbol(arg->GetValue());
        asmGen.mov(r64::rbp, -offset, kArgRegs[argCount]);
        arg = arg->GetLeft();
        ++argCount;
    }
    asmGen.sub(r64::rsp, 8 * argCount);

    CodeGenStmt(node->GetRight());

    asmGen.mov(r64::rsp, r64::rbp);
    asmGen.pop(r64::rbp);

    if (node->GetValue() == kEntryFunctionName) {
        asmGen.mov(r64::rax, 60);
        asmGen.mov(r64::rdi, 0);
        asmGen.syscall();
    }

    asmGen.mov(r64::rax, -1);
    asmGen.ret();

    vars.ExitScope();
}

void CodeGen::EmitSemicolon(Node* node) {
    CodeGenStmt(node->GetLeft());
    CodeGenStmt(node->GetRight());
}

void CodeGen::EmitEqual(Node* node) {
    CodeGenExpr(node->GetRight());
    asmGen.pop(r64::rax);

    int offset = vars.FindSymbol(node->GetLeft()->GetValue());
    if (offset == -1) {
        offset = vars.AddSymbol(node->GetLeft()->GetValue());
        asmGen.sub(r64::rsp, 8);
    }

    asmGen.mov(r64::rbp, -offset, r64::rax);
}

void CodeGen::EmitPrintAscii(Node* node) {
    size_t printAsciiAddr = funcs.FindFunction(keyPrintAscii);
    if (!printAsciiAddr) {
        std::cerr << "Function \"" << keyPrintAscii << "\" is not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    CodeGenExpr(node->GetLeft());
    asmGen.pop(r64::rdi);  

    asmGen.push(r64::rax);
    asmGen.push(r64::rcx);
    asmGen.push(r64::rdx);
    asmGen.push(r64::rsi);
    asmGen.push(r64::rdi);

    asmGen.call((int32_t)(printAsciiAddr - (asmGen.GetCodeSize() + 5)));

    asmGen.pop(r64::rdi);
    asmGen.pop(r64::rsi);
    asmGen.pop(r64::rdx);
    asmGen.pop(r64::rcx);
    asmGen.pop(r64::rax);
}

void CodeGen::EmitPrintInt(Node* node) { 
    size_t printIntAddr = funcs.FindFunction(keyPrintInt);
    if (!printIntAddr) {
        std::cerr << "Function \"" << keyPrintInt << "\" is not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    CodeGenExpr(node->GetLeft());
    asmGen.pop(r64::rdi);  

    asmGen.push(r64::rax);    
    asmGen.push(r64::rcx);
    asmGen.push(r64::rdx);
    asmGen.push(r64::rdi);

    asmGen.call((int32_t)(printIntAddr - (asmGen.GetCodeSize() + 5)));

    asmGen.pop(r64::rdi);
    asmGen.pop(r64::rdx);
    asmGen.pop(r64::rcx);
    asmGen.pop(r64::rax);
}

void CodeGen::EmitIf(Node* node) {
    CodeGenExpr(node->GetLeft());
    asmGen.pop(r64::rax);
    
    asmGen.cmp(r64::rax, 0);
    int32_t jmpPos_1 = (int32_t)asmGen.GetCodeSize();
    asmGen.je(0);

    vars.EnterScope();

    CodeGenStmt(node->GetRight());

    vars.ExitScope();

    int32_t jmpTarget_1 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    asmGen.InsertNumber(jmpOffset_1, jmpPos_1 + 2);
}

void CodeGen::EmitWhile(Node* node) {
    int32_t jmpTarget_2 = (int32_t)asmGen.GetCodeSize();
    
    CodeGenExpr(node->GetLeft());
    asmGen.pop(r64::rax);

    asmGen.cmp(r64::rax, 0);
    int32_t jmpPos_1 = (int32_t)asmGen.GetCodeSize();
    asmGen.je(0);

    vars.EnterScope();

    CodeGenStmt(node->GetRight());

    vars.ExitScope();
    
    int32_t jmpPos_2 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 5);
    asmGen.jmp(jmpOffset_2);

    int32_t jmpTarget_1 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    asmGen.InsertNumber(jmpOffset_1, jmpPos_1 + 2);
}

void CodeGen::EmitReturn(Node* node) {
    CodeGenExpr(node->GetLeft());
    asmGen.pop(r64::rax);

    asmGen.mov(r64::rsp, r64::rbp);
    asmGen.pop(r64::rbp);

    asmGen.ret();
}
