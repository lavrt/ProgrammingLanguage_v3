#include "asmCommands.h"
#include "generator.h"

void CodeGen::CreateStandartFunctions() {
    asmGen.nop(); // it is not allowed to place functions with asmGen.GetCodeSize() = 0

    CreatePrintAscii();
    CreatePrintInt();
    CreateReadInt();
}

void CodeGen::CreatePrintAscii() {
    funcs.AddFunction(keyPrintAscii, asmGen.GetCodeSize());

    asmGen.push(r64::rbp);
    asmGen.mov(r64::rbp, r64::rsp);
    asmGen.sub(r64::rsp, 8);

    asmGen.push(r64::rdi);

    asmGen.mov(r64::rax, 1);
    asmGen.mov(r64::rdi, 1);
    asmGen.mov(r64::rsi, r64::rsp);
    asmGen.mov(r64::rdx, 8);

    asmGen.syscall();

    asmGen.mov(r64::rsp, r64::rbp);
    asmGen.pop(r64::rbp);
    asmGen.ret();
}

void CodeGen::CreatePrintInt() {
    funcs.AddFunction(keyPrintInt, asmGen.GetCodeSize());

    asmGen.push(r64::rbx);

    asmGen.push(r64::rbp);
    asmGen.mov(r64::rbp, r64::rsp);

    asmGen.mov(r64::rcx, 0);
    asmGen.mov(r64::rax, r64::rdi);

    asmGen.cmp(r64::rax, 0); 
    int32_t jmpPos_1 = (int32_t)asmGen.GetCodeSize();
    asmGen.jge(0);

    asmGen.neg(r64::rax); 
                                   
    asmGen.push(r64::rax);
    asmGen.push(r64::rcx);
    asmGen.push(r64::rdx);
    asmGen.push(r64::rsi);
    asmGen.push(r64::rdi);

    asmGen.push('-');

    asmGen.mov(r64::rax, 1);
    asmGen.mov(r64::rdi, 1);
    asmGen.mov(r64::rsi, r64::rsp);
    asmGen.mov(r64::rdx, 8);

    asmGen.syscall();

    asmGen.add(r64::rsp, 8);

    asmGen.pop(r64::rdi);
    asmGen.pop(r64::rsi);
    asmGen.pop(r64::rdx);
    asmGen.pop(r64::rcx);
    asmGen.pop(r64::rax);

    int32_t jmpTarget_1 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    asmGen.InsertNumber(jmpOffset_1, jmpPos_1 + 2);

    int32_t jmpTarget_2 = (int32_t)asmGen.GetCodeSize();

    asmGen.mov(r64::rdx, 0);
    asmGen.mov(r64::rbx, 10);
    asmGen.idiv(r64::rbx);
    asmGen.push(r64::rdx);
    asmGen.inc(r64::rcx);

    asmGen.cmp(r64::rax, 0);
    int32_t jmpPos_2 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 6);
    asmGen.jne(jmpOffset_2);

    int32_t jmpTarget_4 = (int32_t)asmGen.GetCodeSize();

    asmGen.cmp(r64::rcx, 0);
    int32_t jmpPos_3 = (int32_t)asmGen.GetCodeSize();
    asmGen.je(0);

    asmGen.pop(r64::rdi);
    asmGen.add(r64::rdi, '0');

    asmGen.push(r64::rax);
    asmGen.push(r64::rcx);
    asmGen.push(r64::rdx);
    asmGen.push(r64::rsi);

    asmGen.push(r64::rdi);

    asmGen.mov(r64::rax, 1);
    asmGen.mov(r64::rdi, 1);
    asmGen.mov(r64::rsi, r64::rsp);
    asmGen.mov(r64::rdx, 8);

    asmGen.syscall();

    asmGen.pop(r64::rdi);

    asmGen.pop(r64::rsi);
    asmGen.pop(r64::rdx);
    asmGen.pop(r64::rcx);
    asmGen.pop(r64::rax);

    asmGen.dec(r64::rcx);

    int32_t jmpPos_4 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_4 = jmpTarget_4 - (jmpPos_4 + 5);
    asmGen.jmp(jmpOffset_4); 

    int32_t jmpTarget_3 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_3 = jmpTarget_3 - (jmpPos_3 + 6);
    asmGen.InsertNumber(jmpOffset_3, jmpPos_3 + 2);
                                   
    asmGen.push(r64::rax);
    asmGen.push(r64::rcx);
    asmGen.push(r64::rdx);
    asmGen.push(r64::rsi);
    asmGen.push(r64::rdi);

    asmGen.push('\n');

    asmGen.mov(r64::rax, 1);
    asmGen.mov(r64::rdi, 1);
    asmGen.mov(r64::rsi, r64::rsp);
    asmGen.mov(r64::rdx, 8);

    asmGen.syscall();

    asmGen.add(r64::rsp, 8);
    
    asmGen.pop(r64::rdi);
    asmGen.pop(r64::rsi);
    asmGen.pop(r64::rdx);
    asmGen.pop(r64::rcx);
    asmGen.pop(r64::rax);

    asmGen.mov(r64::rsp, r64::rbp);
    asmGen.pop(r64::rbp);

    asmGen.pop(r64::rbx);

    asmGen.ret();
}

void CodeGen::CreateReadInt() {
    funcs.AddFunction(keyReadInt, asmGen.GetCodeSize());

    asmGen.push(r64::rbx);

    asmGen.push(r64::rbp);
    asmGen.mov(r64::rbp, r64::rsp);

    asmGen.sub(r64::rsp, 32);

    asmGen.mov(r64::rax, 0);
    asmGen.mov(r64::rdi, 0);
    asmGen.mov(r64::rsi, r64::rsp);
    asmGen.mov(r64::rdx, 32);

    asmGen.syscall();

    asmGen.mov(r64::rcx, r64::rax);
    asmGen.mov(r64::rax, 0);
    asmGen.mov(r64::rdi, 1);
    asmGen.mov(r64::rbx, r64::rsp);

    int32_t jmpTarget_1 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpTarget_2 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpTarget_3 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpTarget_4 = (int32_t)asmGen.GetCodeSize();

    asmGen.cmp(r64::rcx, 0);
    int32_t jmpPos_7 = (int32_t)asmGen.GetCodeSize();
    asmGen.je(0);

    asmGen.mov(r64::rdx, r64::rbx, 0);
    asmGen.movzx(r64::rdx, r8::dl);

    asmGen.inc(r64::rbx);
    asmGen.dec(r64::rcx);

    asmGen.cmp(r64::rdx, ' ');
    int32_t jmpPos_1 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    asmGen.je(jmpOffset_1);

    asmGen.cmp(r64::rdx, '\n');
    int32_t jmpPos_2 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 6);
    asmGen.je(jmpOffset_2);

    asmGen.cmp(r64::rdx, '-');
    int32_t jmpPos_5 = (int32_t)asmGen.GetCodeSize();
    asmGen.jne(0);

    asmGen.mov(r64::rdi, -1);

    int32_t jmpPos_3 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_3 = jmpTarget_3 - (jmpPos_3 + 5);
    asmGen.jmp(jmpOffset_3);

    int32_t jmpTarget_5 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_5 = jmpTarget_5 - (jmpPos_5 + 6);
    asmGen.InsertNumber(jmpOffset_5, jmpPos_5 + 2);

    asmGen.cmp(r64::rdx, '+');
    int32_t jmpPos_4 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_4 = jmpTarget_4 - (jmpPos_4 + 6);
    asmGen.je(jmpOffset_4);

    asmGen.mov(r64::rsi, 10);

    int32_t jmpTarget_6 = (int32_t)asmGen.GetCodeSize();

    asmGen.cmp(r64::rcx, 0);
    int32_t jmpPos_8 = (int32_t)asmGen.GetCodeSize();
    asmGen.je(0);
    
    asmGen.sub(r64::rdx, '0');

    asmGen.cmp(r64::rdx, 0);
    int32_t jmpPos_9 = (int32_t)asmGen.GetCodeSize();
    asmGen.jl(0);

    asmGen.cmp(r64::rdx, 9);
    int32_t jmpPos_10 = (int32_t)asmGen.GetCodeSize();
    asmGen.jg(0);

    asmGen.imul(r64::rax, r64::rsi);
    asmGen.add(r64::rax, r64::rdx);

    asmGen.mov(r64::rdx, r64::rbx, 0);
    asmGen.movzx(r64::rdx, r8::dl);

    asmGen.inc(r64::rbx);
    asmGen.dec(r64::rcx);

    int32_t jmpPos_6 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_6 = jmpTarget_6 - (jmpPos_6 + 5);
    asmGen.jmp(jmpOffset_6);

    int32_t jmpTarget_7 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_7 = jmpTarget_7 - (jmpPos_7 + 6);
    asmGen.InsertNumber(jmpOffset_7, jmpPos_7 + 2);

    int32_t jmpTarget_8 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_8 = jmpTarget_8 - (jmpPos_8 + 6);
    asmGen.InsertNumber(jmpOffset_8, jmpPos_8 + 2);

    int32_t jmpTarget_9 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_9 = jmpTarget_9 - (jmpPos_9 + 6);
    asmGen.InsertNumber(jmpOffset_9, jmpPos_9 + 2);
    
    int32_t jmpTarget_10 = (int32_t)asmGen.GetCodeSize();
    int32_t jmpOffset_10 = jmpTarget_10 - (jmpPos_10 + 6);
    asmGen.InsertNumber(jmpOffset_10, jmpPos_10 + 2);

    asmGen.imul(r64::rax, r64::rdi);

    asmGen.mov(r64::rsp, r64::rbp);
    asmGen.pop(r64::rbp);

    asmGen.pop(r64::rbx);

    asmGen.ret();
}
