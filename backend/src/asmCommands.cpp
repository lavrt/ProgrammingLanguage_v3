#include "asmCommands.h"

void x86_64::push(TCodeGen* cg, r64 reg) {
    if (reg <= r64::rdi) {
        // opcode: 0x50 + rd
        uint8_t opcode[] = {static_cast<uint8_t>(0x50 + (static_cast<int>(reg) & 0x7))};
        AppendCode(cg, opcode);
    } else {
        // opcode: REX + 0x50 + reg<<3
        // REX.B: 0x41
        uint8_t opcode[] = {0x41, static_cast<uint8_t>(0x50 + static_cast<int>(reg) - static_cast<int>(r64::r8))};
        AppendCode(cg, opcode);
    }
}

void x86_64::push(TCodeGen* cg, int32_t imm) {
    // opcode: 68 id
    uint8_t opcode[] = {0x68};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(imm));
}

void x86_64::pop(TCodeGen* cg, r64 reg) {
    if (reg <= r64::rdi) {
        // opcode: 0x58 + rd
        uint8_t opcode[] = {static_cast<uint8_t>(0x58 + (static_cast<int>(reg) & 0x7))};
        AppendCode(cg, opcode);
    } else {
        // opcode: REX + 0x58 + reg<<3
        // REX.B: 0x41
        uint8_t opcode[] = {0x41, static_cast<uint8_t>(0x58 + static_cast<int>(reg) - static_cast<int>(r64::r8))};
        AppendCode(cg, opcode);
    }
}

void x86_64::mov(TCodeGen* cg, r64 reg, int32_t imm) {
    // opcode: REX.W + C7 /0 imm32
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=000, R/M=reg_code)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x48, 0xc7, modrm}; 
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(imm));
}

void x86_64::mov(TCodeGen* cg, r64 dst, r64 src) {
    // opcode: REX.W + 8B /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(dst) & 0x7) << 3) + (static_cast<int>(src) & 0x7));
    uint8_t opcode[] = {0x48, 0x8b, modrm};
    AppendCode(cg, opcode);
}

void x86_64::mov(TCodeGen* cg, r64 dst, r64 src, int32_t offset) { 
    // opcode: REX.W + 8B /r disp32
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=10, Reg=reg, R/M=src)
    uint8_t modrm = static_cast<uint8_t>(0x80 + ((static_cast<int>(dst) & 0x7) << 3) + (static_cast<int>(src) & 0x7));
    uint8_t opcode[] = {0x48, 0x8b, modrm};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(offset));
}

void x86_64::mov(TCodeGen* cg, r64 src, int32_t offset, r64 dst) { 
    // opcode: REX.W + 89 /r disp32
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=10, Reg=reg, R/M=src)
    uint8_t modrm = static_cast<uint8_t>(0x80 + ((static_cast<int>(dst) & 0x7) << 3) + (static_cast<int>(src) & 0x7));
    uint8_t opcode[] = {0x48, 0x89, modrm};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(offset));
}

void x86_64::add(TCodeGen* cg, r64 dst, r64 src) {
    // opcode: REX.W + 01 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(src) & 0x7) << 3) + (static_cast<int>(dst) & 0x7));
    uint8_t opcode[] = {0x48, 0x01, modrm};
    AppendCode(cg, opcode);
}

void x86_64::add(TCodeGen* cg, r64 reg, int32_t imm) {
    // opcode: REX.W + 81 /0 id
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x48, 0x81, modrm};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(imm));
}

void x86_64::sub(TCodeGen* cg, r64 dst, r64 src) {
    // opcode: REX.W + 29 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(src) & 0x7) << 3) + (static_cast<int>(dst) & 0x7));
    uint8_t opcode[] = {0x48, 0x29, modrm};
    AppendCode(cg, opcode);
}

void x86_64::sub(TCodeGen* cg, r64 reg, int32_t imm) {
    // opcode: REX.W + 81 /5 id
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=101, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xe8 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x48, 0x81, modrm};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(imm));
}

void x86_64::imul(TCodeGen* cg, r64 dst, r64 src) {
    // opcode: REX.W + 0F AF /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(dst) & 0x7) << 3) + (static_cast<int>(src) & 0x7));
    uint8_t opcode[] = {0x48, 0x0f, 0xaf, modrm};
    AppendCode(cg, opcode);
}

void x86_64::idiv(TCodeGen* cg, r64 reg) {
    // opcode: REX.W + F7 /7
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=111, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xf8 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x48, 0xf7, modrm};
    AppendCode(cg, opcode);
}

void x86_64::cmp(TCodeGen* cg, r64 dst, r64 src) {
    // opcode: REX.W + 39 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(src) & 0x7) << 3) + (static_cast<int>(dst) & 0x7));
    uint8_t opcode[] = {0x48, 0x39, modrm};
    AppendCode(cg, opcode);
}

void x86_64::cmp(TCodeGen* cg, r64 reg, int32_t imm) {
    // opcode: REX.W + 81 /7 id
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=111, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xf8 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x48, 0x81, modrm};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(imm));
}

void x86_64::je(TCodeGen* cg, int32_t offset) {
    // opcode: 0F 84 imm32
    uint8_t opcode[] = {0x0f, 0x84};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(offset));
}

void x86_64::jmp(TCodeGen* cg, int32_t offset) {
    // opcode: E9 cd
    uint8_t opcode[] = {0xe9};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(offset));
}

void x86_64::jl(TCodeGen* cg, int32_t offset) {
    // opcode: 0F 8C cd
    uint8_t opcode[] = {0x0f, 0x8c};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(offset));
}

void x86_64::jg(TCodeGen* cg, int32_t offset) {
    // opcode: 0F 8F cd
    uint8_t opcode[] = {0x0f, 0x8f};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(offset));
}

void x86_64::jge(TCodeGen* cg, int32_t offset) {
    // opcode: 0F 8D cd
    uint8_t opcode[] = {0x0f, 0x8d};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(offset));
}

void x86_64::jne(TCodeGen* cg, int32_t offset) {
    // opcode: OF 85 cd
    uint8_t opcode[] = {0x0f, 0x85};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(offset));
}

void x86_64::call(TCodeGen* cg, int32_t offset) {
    // opcode: E8 cd
    uint8_t opcode[] = {0xe8};
    AppendCode(cg, opcode);
    AppendCode(cg, AsBytes(offset));
}

void x86_64::xchg(TCodeGen* cg, r64 dst, r64 src) {
    // opcode: REX.W + 87 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(src) & 0x7) << 3) + (static_cast<int>(dst) & 0x7));
    uint8_t opcode[] = {0x48, 0x87, modrm};
    AppendCode(cg, opcode);
}

void x86_64::neg(TCodeGen* cg, r64 reg) {
    // opcode: REX.W + F7 /3
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=011, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xd8 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x48, 0xf7, modrm};
    AppendCode(cg, opcode);
}

void x86_64::inc(TCodeGen* cg, r64 reg) {
    // opcode: REX.W + FF /0
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x48, 0xff, modrm};
    AppendCode(cg, opcode);
}

void x86_64::dec(TCodeGen* cg, r64 reg) {
    // opcode: REX.W + FF /1
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=001, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc8 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x48, 0xff, modrm};
    AppendCode(cg, opcode);
}

void x86_64::ret(TCodeGen* cg) {
    // opcode: С3
    uint8_t opcode[] = {0xc3};
    AppendCode(cg, opcode);
}

void x86_64::syscall(TCodeGen* cg) {
    // opcode: 0F 05
    uint8_t opcode[] = {0x0f, 0x05};
    AppendCode(cg, opcode);
}

void x86_64::nop(TCodeGen* cg) {
    // opcode: NP 90
    uint8_t opcode[] = {0x90};
    AppendCode(cg, opcode);
}

void x86_64::setg(TCodeGen* cg, r64 reg) {
    // opcode: 0F 9F
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x9f, modrm};
    AppendCode(cg, opcode);
}

void x86_64::setge(TCodeGen* cg, r64 reg) {
    // opcode: 0F 9D
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x9d, modrm};
    AppendCode(cg, opcode);
}

void x86_64::setl(TCodeGen* cg, r64 reg) {
    // opcode: 0F 9C
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x9c, modrm};
    AppendCode(cg, opcode);
}

void x86_64::setle(TCodeGen* cg, r64 reg) {
    // opcode: 0F 9E
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x9e, modrm};
    AppendCode(cg, opcode);
}

void x86_64::sete(TCodeGen* cg, r64 reg) {
    // opcode: 0F 94
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x94, modrm};
    AppendCode(cg, opcode);    
}

void x86_64::setne(TCodeGen* cg, r64 reg) {
    // opcode: 0F 95
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x95, modrm};
    AppendCode(cg, opcode);
}

void x86_64::movzx(TCodeGen* cg, r64 dst, r8 src) {
    // opcode: REX.W + 0F B6 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(dst) & 0x7) << 3) + (static_cast<int>(src) & 0x7));
    uint8_t opcode[] = {0x48, 0x0f, 0xb6, modrm};
    AppendCode(cg, opcode);
}
