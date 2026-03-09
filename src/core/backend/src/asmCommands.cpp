#include "asmCommands.h"

// static ------------------------------------------------------------------------------------------

inline constexpr uint8_t kRexW = 0x48;
inline constexpr uint8_t kRexR = 0x44;
inline constexpr uint8_t kRexX = 0x42;
inline constexpr uint8_t kRexB = 0x41;

void x86_64::push(r64 reg) {
    if (reg <= r64::rdi) {
        // opcode: 0x50 + rd
        uint8_t opcode[] = {static_cast<uint8_t>(0x50 + (static_cast<int>(reg) & 0x7))};
        code.Append(opcode);
    } else {
        // opcode: REX + 0x50 + rd
        uint8_t opcode[] = {kRexB, static_cast<uint8_t>(0x50 + (static_cast<int>(reg) & 0x7))};
        code.Append(opcode);
    }
}

void x86_64::push(int32_t imm) {
    // opcode: 68 id
    uint8_t opcode[] = {0x68};
    code.Append(opcode);
    code.Append(imm);
}

void x86_64::pop(r64 reg) {
    if (reg <= r64::rdi) {
        // opcode: 0x58 + rd
        uint8_t opcode[] = {static_cast<uint8_t>(0x58 + (static_cast<int>(reg) & 0x7))};
        code.Append(opcode);
    } else {
        // opcode: REX + 0x58 + reg<<3
        uint8_t opcode[] = {kRexB, static_cast<uint8_t>(0x58 + static_cast<int>(reg) - static_cast<int>(r64::r8))};
        code.Append(opcode);
    }
}

void x86_64::mov(r64 reg, int32_t imm) {
    // opcode: REX.W + C7 /0 imm32
    // ModR/M: (Mod=11, Reg=000, R/M=reg_code)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {kRexW, 0xc7, modrm}; 
    code.Append(opcode);
    code.Append(imm);
}

void x86_64::mov(r64 dst, r64 src) {
    // opcode: REX.W + 8B /r
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(dst) & 0x7) << 3) + (static_cast<int>(src) & 0x7));
    uint8_t opcode[] = {kRexW, 0x8b, modrm};
    code.Append(opcode);
}

void x86_64::mov(r64 dst, r64 src, int32_t offset) { 
    // opcode: REX.W + 8B /r disp32
    // ModR/M: (Mod=10, Reg=reg, R/M=src)
    uint8_t modrm = static_cast<uint8_t>(0x80 + ((static_cast<int>(dst) & 0x7) << 3) + (static_cast<int>(src) & 0x7));
    uint8_t opcode[] = {kRexW, 0x8b, modrm};
    code.Append(opcode);
    code.Append(offset);
}

void x86_64::mov(r64 src, int32_t offset, r64 dst) {
    // opcode: REX.W + 89 /r disp32
    // ModR/M: (Mod=10, Reg=reg, R/M=src)
    uint8_t rex = static_cast<int>(src) >= 8 || static_cast<int>(dst) >= 8 ? kRexW | kRexR : kRexW;
    uint8_t modrm = static_cast<uint8_t>(0x80 + ((static_cast<int>(dst) & 0x7) << 3) + (static_cast<int>(src) & 0x7));
    uint8_t opcode[] = {rex, 0x89, modrm};
    code.Append(opcode);
    code.Append(offset);
}

void x86_64::add(r64 dst, r64 src) {
    // opcode: REX.W + 01 /r
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(src) & 0x7) << 3) + (static_cast<int>(dst) & 0x7));
    uint8_t opcode[] = {kRexW, 0x01, modrm};
    code.Append(opcode);
}

void x86_64::add(r64 reg, int32_t imm) {
    // opcode: REX.W + 81 /0 id
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {kRexW, 0x81, modrm};
    code.Append(opcode);
    code.Append(imm);
}

void x86_64::sub(r64 dst, r64 src) {
    // opcode: REX.W + 29 /r
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(src) & 0x7) << 3) + (static_cast<int>(dst) & 0x7));
    uint8_t opcode[] = {kRexW, 0x29, modrm};
    code.Append(opcode);
}

void x86_64::sub(r64 reg, int32_t imm) {
    // opcode: REX.W + 81 /5 id
    // ModR/M: (Mod=11, Reg=101, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xe8 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {kRexW, 0x81, modrm};
    code.Append(opcode);
    code.Append(imm);
}

void x86_64::imul(r64 dst, r64 src) {
    // opcode: REX.W + 0F AF /r
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(dst) & 0x7) << 3) + (static_cast<int>(src) & 0x7));
    uint8_t opcode[] = {kRexW, 0x0f, 0xaf, modrm};
    code.Append(opcode);
}

void x86_64::idiv(r64 reg) {
    // opcode: REX.W + F7 /7
    // ModR/M: (Mod=11, Reg=111, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xf8 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {kRexW, 0xf7, modrm};
    code.Append(opcode);
}

void x86_64::cmp(r64 dst, r64 src) {
    // opcode: REX.W + 39 /r
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(src) & 0x7) << 3) + (static_cast<int>(dst) & 0x7));
    uint8_t opcode[] = {kRexW, 0x39, modrm};
    code.Append(opcode);
}

void x86_64::cmp(r64 reg, int32_t imm) {
    // opcode: REX.W + 81 /7 id
    // ModR/M: (Mod=11, Reg=111, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xf8 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {kRexW, 0x81, modrm};
    code.Append(opcode);
    code.Append(imm);
}

void x86_64::je(int32_t offset) {
    // opcode: 0F 84 imm32
    uint8_t opcode[] = {0x0f, 0x84};
    code.Append(opcode);
    code.Append(offset);
}

void x86_64::jmp(int32_t offset) {
    // opcode: E9 cd
    uint8_t opcode[] = {0xe9};
    code.Append(opcode);
    code.Append(offset);
}

void x86_64::jl(int32_t offset) {
    // opcode: 0F 8C cd
    uint8_t opcode[] = {0x0f, 0x8c};
    code.Append(opcode);
    code.Append(offset);
}

void x86_64::jg(int32_t offset) {
    // opcode: 0F 8F cd
    uint8_t opcode[] = {0x0f, 0x8f};
    code.Append(opcode);
    code.Append(offset);
}

void x86_64::jge(int32_t offset) {
    // opcode: 0F 8D cd
    uint8_t opcode[] = {0x0f, 0x8d};
    code.Append(opcode);
    code.Append(offset);
}

void x86_64::jne(int32_t offset) {
    // opcode: OF 85 cd
    uint8_t opcode[] = {0x0f, 0x85};
    code.Append(opcode);
    code.Append(offset);
}

void x86_64::call(int32_t offset) {
    // opcode: E8 cd
    uint8_t opcode[] = {0xe8};
    code.Append(opcode);
    code.Append(offset);
}

void x86_64::xchg(r64 dst, r64 src) {
    // opcode: REX.W + 87 /r
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(src) & 0x7) << 3) + (static_cast<int>(dst) & 0x7));
    uint8_t opcode[] = {kRexW, 0x87, modrm};
    code.Append(opcode);
}

void x86_64::neg(r64 reg) {
    // opcode: REX.W + F7 /3
    // ModR/M: (Mod=11, Reg=011, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xd8 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {kRexW, 0xf7, modrm};
    code.Append(opcode);
}

void x86_64::inc(r64 reg) {
    // opcode: REX.W + FF /0
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {kRexW, 0xff, modrm};
    code.Append(opcode);
}

void x86_64::dec(r64 reg) {
    // opcode: REX.W + FF /1
    // ModR/M: (Mod=11, Reg=001, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc8 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {kRexW, 0xff, modrm};
    code.Append(opcode);
}

void x86_64::ret() {
    // opcode: С3
    uint8_t opcode[] = {0xc3};
    code.Append(opcode);
}

void x86_64::syscall() {
    // opcode: 0F 05
    uint8_t opcode[] = {0x0f, 0x05};
    code.Append(opcode);
}

void x86_64::nop() {
    // opcode: NP 90
    uint8_t opcode[] = {0x90};
    code.Append(opcode);
}

void x86_64::setg(r64 reg) {
    // opcode: 0F 9F
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x9f, modrm};
    code.Append(opcode);
}

void x86_64::setge(r64 reg) {
    // opcode: 0F 9D
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x9d, modrm};
    code.Append(opcode);
}

void x86_64::setl(r64 reg) {
    // opcode: 0F 9C
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x9c, modrm};
    code.Append(opcode);
}

void x86_64::setle(r64 reg) {
    // opcode: 0F 9E
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x9e, modrm};
    code.Append(opcode);
}

void x86_64::sete(r64 reg) {
    // opcode: 0F 94
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x94, modrm};
    code.Append(opcode);    
}

void x86_64::setne(r64 reg) {
    // opcode: 0F 95
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + (static_cast<int>(reg) & 0x7));
    uint8_t opcode[] = {0x0f, 0x95, modrm};
    code.Append(opcode);
}

void x86_64::movzx(r64 dst, r8 src) {
    // opcode: REX.W + 0F B6 /r
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = static_cast<uint8_t>(0xc0 + ((static_cast<int>(dst) & 0x7) << 3) + (static_cast<int>(src) & 0x7));
    uint8_t opcode[] = {kRexW, 0x0f, 0xb6, modrm};
    code.Append(opcode);
}
