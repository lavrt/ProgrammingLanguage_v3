#ifndef ASM_COMMANDS_H
#define ASM_COMMANDS_H

#include "generator.h"

class x86_64 {
private:
    template <typename T>
    static std::span<uint8_t> AsBytes(T& value) {
        return std::span<uint8_t>(
            reinterpret_cast<uint8_t*>(&value),
            sizeof(T)
        );
    }

public:
    enum class r64 {
        rax = 0,    rcx = 1,    rdx = 2,    rbx = 3,    rsp = 4,    rbp = 5,    rsi = 6,    rdi = 7,
        r8 = 8,     r9 = 9,     r10 = 10,   r11 = 11,   r12 = 12,   r13 = 13,   r14 = 14,   r15 = 15,
    };

    enum class r8 {
        al = 0,     cl = 1,     dl = 2,     bl = 3,     spl = 4,    bpl = 5,    sil = 6,    dil = 7, 
        r8b = 8,    r9b = 9,    r10b = 10,  r11b = 11,  r12b = 12,  r13b = 13,  r14b = 14,  r15b = 15,
    };

    static void push(TCodeGen* cg, r64 reg);
    static void push(TCodeGen* cg, int32_t imm);
    static void pop(TCodeGen* cg, r64 reg);
    static void mov(TCodeGen* cg, r64 reg, int32_t imm);
    static void mov(TCodeGen* cg, r64 dst, r64 src);
    static void mov(TCodeGen* cg, r64 dst, r64 src, int32_t offset);
    static void mov(TCodeGen* cg, r64 src, int32_t offset, r64 dst);
    static void add(TCodeGen* cg, r64 dst, r64 src);
    static void add(TCodeGen* cg, r64 reg, int32_t imm);
    static void sub(TCodeGen* cg, r64 dst, r64 src);
    static void sub(TCodeGen* cg, r64 reg, int32_t imm);
    static void imul(TCodeGen* cg, r64 dst, r64 src);
    static void idiv(TCodeGen* cg, r64 reg);
    static void cmp(TCodeGen* cg, r64 dst, r64 src);
    static void cmp(TCodeGen* cg, r64 reg, int32_t imm);
    static void je(TCodeGen* cg, int32_t offset);
    static void jmp(TCodeGen* cg, int32_t offset);
    static void jl(TCodeGen* cg, int32_t offset);
    static void jg(TCodeGen* cg, int32_t offset);
    static void jge(TCodeGen* cg, int32_t offset);
    static void jne(TCodeGen* cg, int32_t offset);
    static void call(TCodeGen* cg, int32_t offset);
    static void xchg(TCodeGen* cg, r64 dst, r64 src);
    static void neg(TCodeGen* cg, r64 reg);
    static void inc(TCodeGen* cg, r64 reg);
    static void dec(TCodeGen* cg, r64 reg);
    static void ret(TCodeGen* cg);
    static void syscall(TCodeGen* cg);
    static void nop(TCodeGen* cg);
    static void setg(TCodeGen* cg, r64 reg);
    static void setge(TCodeGen* cg, r64 reg);
    static void setl(TCodeGen* cg, r64 reg);
    static void setle(TCodeGen* cg, r64 reg);
    static void sete(TCodeGen* cg, r64 reg);
    static void setne(TCodeGen* cg, r64 reg);
    static void movzx(TCodeGen* cg, r64 dst, r8 src);
};

#endif // ASM_COMMANDS_H
