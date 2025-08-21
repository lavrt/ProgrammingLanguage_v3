#ifndef ASM_COMMANDS_H
#define ASM_COMMANDS_H

#include <cstdint>
#include <vector>
#include <span>

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

class CodeBuffer {
private:
    std::vector<uint8_t> vec;

public:
    void Append(std::span<uint8_t> data) {
        vec.insert(vec.end(), data.begin(), data.end());
    }

    void Append(int32_t num) {
        std::span<uint8_t> data {
            reinterpret_cast<uint8_t*>(&num),
            sizeof(num)
        };
        vec.insert(vec.end(), data.begin(), data.end());
    }

    void InsertNumber(int32_t num, size_t pos) {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(&num);
        std::copy(data, data + sizeof(num), vec.begin() + pos);
    }

    size_t GetSize() const noexcept {
        return vec.size();
    }

    const uint8_t* GetData() const noexcept {
        return vec.data();
    }
};

class x86_64 {
private:
    CodeBuffer code;

public:
    size_t GetCodeSize() const noexcept {
        return code.GetSize();
    }

    const uint8_t* GetCodeData() const noexcept {
        return code.GetData();
    }

    void InsertNumber(int32_t num, size_t pos) {
        code.InsertNumber(num, pos);
    }

    void push(r64 reg);
    void push(int32_t imm);
    void pop(r64 reg);
    void mov(r64 reg, int32_t imm);
    void mov(r64 dst, r64 src);
    void mov(r64 dst, r64 src, int32_t offset);
    void mov(r64 src, int32_t offset, r64 dst);
    void add(r64 dst, r64 src);
    void add(r64 reg, int32_t imm);
    void sub(r64 dst, r64 src);
    void sub(r64 reg, int32_t imm);
    void imul(r64 dst, r64 src);
    void idiv(r64 reg);
    void cmp(r64 dst, r64 src);
    void cmp(r64 reg, int32_t imm);
    void je(int32_t offset);
    void jmp(int32_t offset);
    void jl(int32_t offset);
    void jg(int32_t offset);
    void jge(int32_t offset);
    void jne(int32_t offset);
    void call(int32_t offset);
    void xchg(r64 dst, r64 src);
    void neg(r64 reg);
    void inc(r64 reg);
    void dec(r64 reg);
    void ret();
    void syscall();
    void nop();
    void setg(r64 reg);
    void setge(r64 reg);
    void setl(r64 reg);
    void setle(r64 reg);
    void sete(r64 reg);
    void setne(r64 reg);
    void movzx(r64 dst, r8 src);
};

#endif // ASM_COMMANDS_H
