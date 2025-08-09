#include "asmCommands.h"

#include "generator.h"

// PUSH r64
// size: 1 byte
void push_reg(TCodeGen* cg, ERegister reg) {
    if (reg <= REG_DI) {
        // opcode: 0x50 + rd
        uint8_t opcode[] = {(uint8_t)(0x50 + (reg & 0x7))};
        AppendCode(cg, opcode, 1);
    } else {
        // opcode: REX + 0x50 + reg<<3
        // REX.B: 0x41
        uint8_t opcode[] = {0x41, (uint8_t)(0x50 + reg - REG_R8)};
        AppendCode(cg, opcode, 2);
    }
}

// PUSH imm32
// size: 5 byte
void push_imm32(TCodeGen* cg, int32_t imm) {
    // opcode: 68 id
    uint8_t opcode[] = {0x68};
    AppendCode(cg, opcode, 1);
    AppendCode(cg, (uint8_t*)&imm, 4);
}

// POP r64
// size: 1 byte
void pop_reg(TCodeGen* cg, ERegister reg) {
    if (reg <= REG_DI) {
        // opcode: 0x58 + rd
        uint8_t opcode = 0x58 + (reg & 0x7);
        AppendCode(cg, &opcode, 1);
    } else {
        // opcode: REX + 0x58 + reg<<3
        // REX.B: 0x41
        uint8_t opcode[] = {0x41, (uint8_t)(0x58 + reg - REG_R8)};
        AppendCode(cg, opcode, 2);
    }
}

// MOV r/m64, imm32
// size: 7 byte
void mov_reg_imm32(TCodeGen* cg, ERegister reg, int32_t imm) {
    // opcode: REX.W + C7 /0 imm32
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=000, R/M=reg_code)
    uint8_t modrm = (uint8_t)(0xc0 + (reg & 0x7));
    uint8_t opcode[] = {0x48, 0xc7, modrm}; 
    AppendCode(cg, opcode, 3);
    AppendCode(cg, (uint8_t*)&imm, 4);
}

// MOV r64, r/m64
// size: 3 byte
void mov_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 8B /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = (uint8_t)(0xc0 + ((dst & 0x7) << 3) + (src & 0x7));
    uint8_t opcode[] = {0x48, 0x8b, modrm};
    AppendCode(cg, opcode, 3);
}

// MOV r64, [rbp-offset]
// size: 7 byte
void mov_reg_mem(TCodeGen* cg, ERegister reg, int32_t offset) { 
    // opcode: REX.W + 8B /r disp32
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=10, Reg=reg, R/M=101(rbp))
    uint8_t modrm = (uint8_t)(0x85 + ((reg & 0x7) << 3));
    uint8_t opcode[] = {0x48, 0x8b, modrm};
    AppendCode(cg, opcode, 3);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// MOV r8, r/m8
// size: 2 byte
void mov_reg8_mem(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX + 8A /r
    // ModR/M: (Mod=00, Reg=dst, R/M=src)
    uint8_t modrm = (uint8_t)(((dst & 0x7) << 3) + (src & 0x7));
    uint8_t opcode[] = {0x8a, modrm};
    AppendCode(cg, opcode, 2);
}

// MOV [rbp-offset], r64
// size: 7 byte
void mov_mem_reg(TCodeGen* cg, int32_t offset, ERegister reg) { 
    // opcode: REX.W + 89 /r disp32
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=10, Reg=reg, R/M=101(rbp))
    uint8_t modrm = (uint8_t)(0x85 + ((reg & 0x7) << 3));
    uint8_t opcode[] = {0x48, 0x89, modrm};
    AppendCode(cg, opcode, 3);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// ADD r/m64, r64
// size: 3 byte
void add_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 01 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = (uint8_t)(0xc0 + ((src & 0x7) << 3) + (dst & 0x7));
    uint8_t opcode[] = {0x48, 0x01, modrm};
    AppendCode(cg, opcode, 3);
}

// ADD r/m64, imm32
// size: 7 byte
void add_reg_imm32(TCodeGen* cg, ERegister reg, int32_t imm) {
    // opcode: REX.W + 81 /0 id
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = (uint8_t)(0xc0 + (reg & 0x07));
    uint8_t opcode[] = {0x48, 0x81, modrm};
    AppendCode(cg, opcode, 3);
    AppendCode(cg, (uint8_t*)&imm, 4);
}

// SUB r/m64, r64
// size: 3 byte
void sub_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 29 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = (uint8_t)(0xc0 + ((src & 0x7) << 3) + (dst & 0x7));
    uint8_t opcode[] = {0x48, 0x29, modrm};
    AppendCode(cg, opcode, 3);
}

// SUB r/m64, imm32
// size: 7 byte
void sub_reg_imm32(TCodeGen* cg, ERegister reg, int32_t imm) {
    // opcode: REX.W + 81 /5 id
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=101, R/M=reg)
    uint8_t modrm = (uint8_t)(0xe8 + (reg & 0x07));
    uint8_t opcode[] = {0x48, 0x81, modrm};
    AppendCode(cg, opcode, 3);
    AppendCode(cg, (uint8_t*)&imm, 4);
}

// IMUL r64, r/m64
// size: 4 byte
void imul_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 0F AF /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = (uint8_t)(0xc0 + ((dst & 0x7) << 3) + (src & 0x7));
    uint8_t opcode[] = {0x48, 0x0f, 0xaf, modrm};
    AppendCode(cg, opcode, 4);
}

// IDIV r/m64
// size: 3 byte
void idiv_reg(TCodeGen* cg, ERegister reg) {
    // opcode: REX.W + F7 /7
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=111, R/M=reg)
    uint8_t modrm = (uint8_t)(0xf8 + (reg & 0x7));
    uint8_t opcode[] = {0x48, 0xf7, modrm};
    AppendCode(cg, opcode, 3);
}

// CMP r/m64, r64
// size: 3 byte
void cmp_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 39 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = (uint8_t)(0xc0 + ((src & 0x7) << 3) + (dst & 0x7));
    uint8_t opcode[] = {0x48, 0x39, modrm};
    AppendCode(cg, opcode, 3);
}

// CMP r/m64, imm32
// size: 7 byte
void cmp_reg_imm32(TCodeGen* cg, ERegister reg, int32_t imm) {
    // opcode: REX.W + 81 /7 id
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=111, R/M=reg)
    uint8_t modrm = (uint8_t)(0xf8 + (reg & 0x07));
    uint8_t opcode[] = {0x48, 0x81, modrm};
    AppendCode(cg, opcode, 3);
    AppendCode(cg, (uint8_t*)&imm, 4);
}

// JE rel32
// size: 6 byte
void je_rel32(TCodeGen* cg, int32_t offset) {
    // opcode: 0F 84 imm32
    uint8_t opcode[] = {0x0f, 0x84};
    AppendCode(cg, opcode, 2);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// JMP rel32
// size: 5 byte
void jmp_rel32(TCodeGen* cg, int32_t offset) {
    // opcode: E9 cd
    uint8_t opcode[] = {0xe9};
    AppendCode(cg, opcode, 1);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// JL rel32
// size: 6 byte
void jl_rel32(TCodeGen* cg, int32_t offset) {
    // opcode: 0F 8C cd
    uint8_t opcode[] = {0x0f, 0x8c};
    AppendCode(cg, opcode, 2);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// JG rel32
// size: 6 byte
void jg_rel32(TCodeGen* cg, int32_t offset) {
    // opcode: 0F 8F cd
    uint8_t opcode[] = {0x0f, 0x8f};
    AppendCode(cg, opcode, 2);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// JGE rel32
// size: 6 byte
void jge_rel32(TCodeGen* cg, int32_t offset) {
    // opcode: 0F 8D cd
    uint8_t opcode[] = {0x0f, 0x8d};
    AppendCode(cg, opcode, 2);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// JNE rel32
// size: 6 byte
void jne_rel32(TCodeGen* cg, int32_t offset) {
    // opcode: OF 85 cd
    uint8_t opcode[] = {0x0f, 0x85};
    AppendCode(cg, opcode, 2);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// CALL rel32
// size: 5 byte
void call_rel32(TCodeGen* cg, int32_t offset) {
    // opcode: E8 cd
    uint8_t opcode[] = {0xe8};
    AppendCode(cg, opcode, 1);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// XOR r/m64, r64
// size: 3 byte
void xor_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 31 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = (uint8_t)(0xc0 + ((src & 0x07) << 3) + (dst & 0x07));
    uint8_t opcode[] = {0x48, 0x31, modrm};
    AppendCode(cg, opcode, 3);
}

// XCHG r/m64, r64
// size: 3 byte
void xchg_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 87 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = (uint8_t)(0xc0 + ((src & 0x07) << 3) + (dst & 0x07));
    uint8_t opcode[] = {0x48, 0x87, modrm};
    AppendCode(cg, opcode, 3);
}

// NEG r/m64
// size: 3 byte
void neg_reg(TCodeGen* cg, ERegister reg) {
    // opcode: REX.W + F7 /3
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=011, R/M=reg)
    uint8_t modrm = (uint8_t)(0xd8 + (reg & 0x07));
    uint8_t opcode[] = {0x48, 0xf7, modrm};
    AppendCode(cg, opcode, 3);
}

// INC r/m64
// size: 3 byte
void inc_reg(TCodeGen* cg, ERegister reg) {
    // opcode: REX.W + FF /0
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = (uint8_t)(0xc0 + (reg & 0x07));
    uint8_t opcode[] = {0x48, 0xff, modrm};
    AppendCode(cg, opcode, 3);
}

// DEC r/m64
// size: 3 byte
void dec_reg(TCodeGen* cg, ERegister reg) {
    // opcode: REX.W + FF /1
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=001, R/M=reg)
    uint8_t modrm = (uint8_t)(0xc8 + (reg & 0x07));
    uint8_t opcode[] = {0x48, 0xff, modrm};
    AppendCode(cg, opcode, 3);
}

// RET
// size: 1 byte
void ret(TCodeGen* cg) {
    // opcode: С3
    uint8_t opcode[] = {0xc3};
    AppendCode(cg, opcode, 1);
}

// SYSCALL
// size: 2 byte
void syscall(TCodeGen* cg) {
    // opcode: 0F 05
    uint8_t opcode[] = {0x0f, 0x05};
    AppendCode(cg, opcode, 2);
}

// NOP
// size: 1 byte
void nop(TCodeGen* cg) {
    // opcode: NP 90
    uint8_t opcode[] = {0x90};
    AppendCode(cg, opcode, 1);
}

// SETG r/m8
// size: 3 byte
void setg_reg(TCodeGen* cg, ERegister reg) {
    // opcode: 0F 9F
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = (uint8_t)(0xc0 + (reg & 0x07));
    uint8_t opcode[] = {0x0f, 0x9f, modrm};
    AppendCode(cg, opcode, 3);
}

// SETGE r/m8
// size: 3 byte
void setge_reg(TCodeGen* cg, ERegister reg) {
    // opcode: 0F 9D
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = (uint8_t)(0xc0 + (reg & 0x07));
    uint8_t opcode[] = {0x0f, 0x9d, modrm};
    AppendCode(cg, opcode, 3);
}

// SETL r/m8
// size: 3 byte
void setl_reg(TCodeGen* cg, ERegister reg) {
    // opcode: 0F 9C
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = (uint8_t)(0xc0 + (reg & 0x07));
    uint8_t opcode[] = {0x0f, 0x9c, modrm};
    AppendCode(cg, opcode, 3);
}

// SETLE r/m8
// size: 3 byte
void setle_reg(TCodeGen* cg, ERegister reg) {
    // opcode: 0F 9E
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = (uint8_t)(0xc0 + (reg & 0x07));
    uint8_t opcode[] = {0x0f, 0x9e, modrm};
    AppendCode(cg, opcode, 3);
}

// SETE r/m8
// size: 3 byte
void sete_reg(TCodeGen* cg, ERegister reg) {
    // opcode: 0F 94
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = (uint8_t)(0xc0 + (reg & 0x07));
    uint8_t opcode[] = {0x0f, 0x94, modrm};
    AppendCode(cg, opcode, 3);    
}


// SETNE r/m8
// size: 3 byte
void setne_reg(TCodeGen* cg, ERegister reg) {
    // opcode: 0F 95
    // ModR/M: (Mod=11, Reg=000, R/M=reg)
    uint8_t modrm = (uint8_t)(0xc0 + (reg & 0x07));
    uint8_t opcode[] = {0x0f, 0x95, modrm};
    AppendCode(cg, opcode, 3);
}

// MOVZX r64, r/m8 
// size: 4 byte
// note: r/m8 can not be encoded to access the following byte registers: AH, BH, CH, DH.
void movzx_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 0F B6 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = (uint8_t)(0xc0 + ((dst & 0x07) << 3) + (src & 0x07));
    const uint8_t opcode[] = {0x48, 0x0f, 0xb6, modrm};
    AppendCode(cg, opcode, 4);
}
