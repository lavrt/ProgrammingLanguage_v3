#ifndef ASM_COMMANDS_H
#define ASM_COMMANDS_H

#include "generator.h"

class x86_64 {
private:


public:
    enum class r64 {
        rax = 0,
        rcx = 1,
        rdx = 2,
        rbx = 3,
        rsp = 4,
        rbp = 5,
        rsi = 6,
        rdi = 7,
        r8 = 8,
        r9 = 9,
        r10 = 10,
        r11 = 11,
        r12 = 12,
        r13 = 13,
        r14 = 14,
        r15 = 15,
    };

    enum class r8 {
        al = 0,     
        cl = 1,     
        dl = 2,     
        bl = 3,     
        spl = 4,    
        bpl = 5,    
        sil = 6,    
        dil = 7, 
        r8b = 8,    
        r9b = 9,    
        r10b = 10,  
        r11b = 11,  
        r12b = 12,  
        r13b = 13,
        r14b = 14,
        r15b = 15,
    };

    static void push(CodeGen* cg, r64 reg);
    static void push(CodeGen* cg, int32_t imm);
    static void pop(CodeGen* cg, r64 reg);
    static void mov(CodeGen* cg, r64 reg, int32_t imm);
    static void mov(CodeGen* cg, r64 dst, r64 src);
    static void mov(CodeGen* cg, r64 dst, r64 src, int32_t offset);
    static void mov(CodeGen* cg, r64 src, int32_t offset, r64 dst);
    static void add(CodeGen* cg, r64 dst, r64 src);
    static void add(CodeGen* cg, r64 reg, int32_t imm);
    static void sub(CodeGen* cg, r64 dst, r64 src);
    static void sub(CodeGen* cg, r64 reg, int32_t imm);
    static void imul(CodeGen* cg, r64 dst, r64 src);
    static void idiv(CodeGen* cg, r64 reg);
    static void cmp(CodeGen* cg, r64 dst, r64 src);
    static void cmp(CodeGen* cg, r64 reg, int32_t imm);
    static void je(CodeGen* cg, int32_t offset);
    static void jmp(CodeGen* cg, int32_t offset);
    static void jl(CodeGen* cg, int32_t offset);
    static void jg(CodeGen* cg, int32_t offset);
    static void jge(CodeGen* cg, int32_t offset);
    static void jne(CodeGen* cg, int32_t offset);
    static void call(CodeGen* cg, int32_t offset);
    static void xchg(CodeGen* cg, r64 dst, r64 src);
    static void neg(CodeGen* cg, r64 reg);
    static void inc(CodeGen* cg, r64 reg);
    static void dec(CodeGen* cg, r64 reg);
    static void ret(CodeGen* cg);
    static void syscall(CodeGen* cg);
    static void nop(CodeGen* cg);
    static void setg(CodeGen* cg, r64 reg);
    static void setge(CodeGen* cg, r64 reg);
    static void setl(CodeGen* cg, r64 reg);
    static void setle(CodeGen* cg, r64 reg);
    static void sete(CodeGen* cg, r64 reg);
    static void setne(CodeGen* cg, r64 reg);
    static void movzx(CodeGen* cg, r64 dst, r8 src);
};

#endif // ASM_COMMANDS_H
