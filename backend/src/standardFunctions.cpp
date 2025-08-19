#include "standardFunctions.h"

#include "asmCommands.h"

// static ------------------------------------------------------------------------------------------

static void CreatePrintAscii(CodeGen* cg);
static void CreatePrintInt(CodeGen* cg);
static void CreateReadInt(CodeGen* cg);

// global ------------------------------------------------------------------------------------------

void CreateStandartFunctions(CodeGen* cg) {
    x86_64::nop(cg); // it is not allowed to place functions with cg->code.GetSize() = 0

    CreatePrintAscii(cg);
    CreatePrintInt(cg);
    CreateReadInt(cg);
}

// static ------------------------------------------------------------------------------------------

static void CreatePrintAscii(CodeGen* cg) {
    cg->funcs.AddFunction(keyPrintAscii, cg->code.GetSize());

    x86_64::push(cg, x86_64::r64::rbp);
    x86_64::mov(cg, x86_64::r64::rbp, x86_64::r64::rsp);
    x86_64::sub(cg, x86_64::r64::rsp, 8);

    x86_64::push(cg, x86_64::r64::rdi);

    x86_64::mov(cg, x86_64::r64::rax, 1);
    x86_64::mov(cg, x86_64::r64::rdi, 1);
    x86_64::mov(cg, x86_64::r64::rsi, x86_64::r64::rsp);
    x86_64::mov(cg, x86_64::r64::rdx, 8);

    x86_64::syscall(cg);

    x86_64::mov(cg, x86_64::r64::rsp, x86_64::r64::rbp);
    x86_64::pop(cg, x86_64::r64::rbp);
    x86_64::ret(cg);
}

static void CreatePrintInt(CodeGen* cg) {
    cg->funcs.AddFunction(keyPrintInt, cg->code.GetSize());

    x86_64::push(cg, x86_64::r64::rbx);

    x86_64::push(cg, x86_64::r64::rbp);
    x86_64::mov(cg, x86_64::r64::rbp, x86_64::r64::rsp);

    x86_64::mov(cg, x86_64::r64::rcx, 0);
    x86_64::mov(cg, x86_64::r64::rax, x86_64::r64::rdi);

    x86_64::cmp(cg, x86_64::r64::rax, 0); 
    int32_t jmpPos_1 = (int32_t)cg->code.GetSize();
    x86_64::jge(cg, 0);

    x86_64::neg(cg, x86_64::r64::rax); 
                                   
    x86_64::push(cg, x86_64::r64::rax);
    x86_64::push(cg, x86_64::r64::rcx);
    x86_64::push(cg, x86_64::r64::rdx);
    x86_64::push(cg, x86_64::r64::rsi);
    x86_64::push(cg, x86_64::r64::rdi);

    x86_64::push(cg, '-');

    x86_64::mov(cg, x86_64::r64::rax, 1);
    x86_64::mov(cg, x86_64::r64::rdi, 1);
    x86_64::mov(cg, x86_64::r64::rsi, x86_64::r64::rsp);
    x86_64::mov(cg, x86_64::r64::rdx, 8);

    x86_64::syscall(cg);

    x86_64::add(cg, x86_64::r64::rsp, 8);

    x86_64::pop(cg, x86_64::r64::rdi);
    x86_64::pop(cg, x86_64::r64::rsi);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rax);

    int32_t jmpTarget_1 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    cg->code.InsertNumber(jmpOffset_1, jmpPos_1 + 2);

    int32_t jmpTarget_2 = (int32_t)cg->code.GetSize();

    x86_64::mov(cg, x86_64::r64::rdx, 0);
    x86_64::mov(cg, x86_64::r64::rbx, 10);
    x86_64::idiv(cg, x86_64::r64::rbx);
    x86_64::push(cg, x86_64::r64::rdx);
    x86_64::inc(cg, x86_64::r64::rcx);

    x86_64::cmp(cg, x86_64::r64::rax, 0);
    int32_t jmpPos_2 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 6);
    x86_64::jne(cg, jmpOffset_2);

    int32_t jmpTarget_4 = (int32_t)cg->code.GetSize();

    x86_64::cmp(cg, x86_64::r64::rcx, 0);
    int32_t jmpPos_3 = (int32_t)cg->code.GetSize();
    x86_64::je(cg, 0);

    x86_64::pop(cg, x86_64::r64::rdi);
    x86_64::add(cg, x86_64::r64::rdi, '0');

    x86_64::push(cg, x86_64::r64::rax);
    x86_64::push(cg, x86_64::r64::rcx);
    x86_64::push(cg, x86_64::r64::rdx);
    x86_64::push(cg, x86_64::r64::rsi);

    x86_64::push(cg, x86_64::r64::rdi);

    x86_64::mov(cg, x86_64::r64::rax, 1);
    x86_64::mov(cg, x86_64::r64::rdi, 1);
    x86_64::mov(cg, x86_64::r64::rsi, x86_64::r64::rsp);
    x86_64::mov(cg, x86_64::r64::rdx, 8);

    x86_64::syscall(cg);

    x86_64::pop(cg, x86_64::r64::rdi);

    x86_64::pop(cg, x86_64::r64::rsi);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::dec(cg, x86_64::r64::rcx);

    int32_t jmpPos_4 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_4 = jmpTarget_4 - (jmpPos_4 + 5);
    x86_64::jmp(cg, jmpOffset_4); 

    int32_t jmpTarget_3 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_3 = jmpTarget_3 - (jmpPos_3 + 6);
    cg->code.InsertNumber(jmpOffset_3, jmpPos_3 + 2);
                                   
    x86_64::push(cg, x86_64::r64::rax);
    x86_64::push(cg, x86_64::r64::rcx);
    x86_64::push(cg, x86_64::r64::rdx);
    x86_64::push(cg, x86_64::r64::rsi);
    x86_64::push(cg, x86_64::r64::rdi);

    x86_64::push(cg, '\n');

    x86_64::mov(cg, x86_64::r64::rax, 1);
    x86_64::mov(cg, x86_64::r64::rdi, 1);
    x86_64::mov(cg, x86_64::r64::rsi, x86_64::r64::rsp);
    x86_64::mov(cg, x86_64::r64::rdx, 8);

    x86_64::syscall(cg);

    x86_64::add(cg, x86_64::r64::rsp, 8);
    
    x86_64::pop(cg, x86_64::r64::rdi);
    x86_64::pop(cg, x86_64::r64::rsi);
    x86_64::pop(cg, x86_64::r64::rdx);
    x86_64::pop(cg, x86_64::r64::rcx);
    x86_64::pop(cg, x86_64::r64::rax);

    x86_64::mov(cg, x86_64::r64::rsp, x86_64::r64::rbp);
    x86_64::pop(cg, x86_64::r64::rbp);

    x86_64::pop(cg, x86_64::r64::rbx);

    x86_64::ret(cg);
}

static void CreateReadInt(CodeGen* cg) {
    cg->funcs.AddFunction(keyReadInt, cg->code.GetSize());

    x86_64::push(cg, x86_64::r64::rbx);

    x86_64::push(cg, x86_64::r64::rbp);
    x86_64::mov(cg, x86_64::r64::rbp, x86_64::r64::rsp);

    x86_64::sub(cg, x86_64::r64::rsp, 32);

    x86_64::mov(cg, x86_64::r64::rax, 0);
    x86_64::mov(cg, x86_64::r64::rdi, 0);
    x86_64::mov(cg, x86_64::r64::rsi, x86_64::r64::rsp);
    x86_64::mov(cg, x86_64::r64::rdx, 32);

    x86_64::syscall(cg);

    x86_64::mov(cg, x86_64::r64::rcx, x86_64::r64::rax);
    x86_64::mov(cg, x86_64::r64::rax, 0);
    x86_64::mov(cg, x86_64::r64::rdi, 1);
    x86_64::mov(cg, x86_64::r64::rbx, x86_64::r64::rsp);

    int32_t jmpTarget_1 = (int32_t)cg->code.GetSize();
    int32_t jmpTarget_2 = (int32_t)cg->code.GetSize();
    int32_t jmpTarget_3 = (int32_t)cg->code.GetSize();
    int32_t jmpTarget_4 = (int32_t)cg->code.GetSize();

    x86_64::cmp(cg, x86_64::r64::rcx, 0);
    int32_t jmpPos_7 = (int32_t)cg->code.GetSize();
    x86_64::je(cg, 0);

    x86_64::mov(cg, x86_64::r64::rdx, x86_64::r64::rbx, 0);
    x86_64::movzx(cg, x86_64::r64::rdx, x86_64::r8::dl);

    x86_64::inc(cg, x86_64::r64::rbx);
    x86_64::dec(cg, x86_64::r64::rcx);

    x86_64::cmp(cg, x86_64::r64::rdx, ' ');
    int32_t jmpPos_1 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    x86_64::je(cg, jmpOffset_1);

    x86_64::cmp(cg, x86_64::r64::rdx, '\n');
    int32_t jmpPos_2 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 6);
    x86_64::je(cg, jmpOffset_2);

    x86_64::cmp(cg, x86_64::r64::rdx, '-');
    int32_t jmpPos_5 = (int32_t)cg->code.GetSize();
    x86_64::jne(cg, 0);

    x86_64::mov(cg, x86_64::r64::rdi, -1);

    int32_t jmpPos_3 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_3 = jmpTarget_3 - (jmpPos_3 + 5);
    x86_64::jmp(cg, jmpOffset_3);

    int32_t jmpTarget_5 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_5 = jmpTarget_5 - (jmpPos_5 + 6);
    cg->code.InsertNumber(jmpOffset_5, jmpPos_5 + 2);

    x86_64::cmp(cg, x86_64::r64::rdx, '+');
    int32_t jmpPos_4 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_4 = jmpTarget_4 - (jmpPos_4 + 6);
    x86_64::je(cg, jmpOffset_4);

    x86_64::mov(cg, x86_64::r64::rsi, 10);

    int32_t jmpTarget_6 = (int32_t)cg->code.GetSize();

    x86_64::cmp(cg, x86_64::r64::rcx, 0);
    int32_t jmpPos_8 = (int32_t)cg->code.GetSize();
    x86_64::je(cg, 0);
    
    x86_64::sub(cg, x86_64::r64::rdx, '0');

    x86_64::cmp(cg, x86_64::r64::rdx, 0);
    int32_t jmpPos_9 = (int32_t)cg->code.GetSize();
    x86_64::jl(cg, 0);

    x86_64::cmp(cg, x86_64::r64::rdx, 9);
    int32_t jmpPos_10 = (int32_t)cg->code.GetSize();
    x86_64::jg(cg, 0);

    x86_64::imul(cg, x86_64::r64::rax, x86_64::r64::rsi);
    x86_64::add(cg, x86_64::r64::rax, x86_64::r64::rdx);

    x86_64::mov(cg, x86_64::r64::rdx, x86_64::r64::rbx, 0);
    x86_64::movzx(cg, x86_64::r64::rdx, x86_64::r8::dl);

    x86_64::inc(cg, x86_64::r64::rbx);
    x86_64::dec(cg, x86_64::r64::rcx);

    int32_t jmpPos_6 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_6 = jmpTarget_6 - (jmpPos_6 + 5);
    x86_64::jmp(cg, jmpOffset_6);

    int32_t jmpTarget_7 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_7 = jmpTarget_7 - (jmpPos_7 + 6);
    cg->code.InsertNumber(jmpOffset_7, jmpPos_7 + 2);

    int32_t jmpTarget_8 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_8 = jmpTarget_8 - (jmpPos_8 + 6);
    cg->code.InsertNumber(jmpOffset_8, jmpPos_8 + 2);

    int32_t jmpTarget_9 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_9 = jmpTarget_9 - (jmpPos_9 + 6);
    cg->code.InsertNumber(jmpOffset_9, jmpPos_9 + 2);
    
    int32_t jmpTarget_10 = (int32_t)cg->code.GetSize();
    int32_t jmpOffset_10 = jmpTarget_10 - (jmpPos_10 + 6);
    cg->code.InsertNumber(jmpOffset_10, jmpPos_10 + 2);

    x86_64::imul(cg, x86_64::r64::rax, x86_64::r64::rdi);

    x86_64::mov(cg, x86_64::r64::rsp, x86_64::r64::rbp);
    x86_64::pop(cg, x86_64::r64::rbp);

    x86_64::pop(cg, x86_64::r64::rbx);

    x86_64::ret(cg);
}
