#include "asmCommands.h"

#include "generator.h"

// PUSH r64
void push_reg(TCodeGen* cg, ERegister reg) {
    // opcode: 0x50 + reg
    uint8_t opcode = 0x50 + (reg & 0x7);
    AppendCode(cg, &opcode, 1);
}

// POP r64
void pop_reg(TCodeGen* cg, ERegister reg) {
    // opcode: 0x58 + reg
    uint8_t opcode = 0x58 + (reg & 0x7);
    AppendCode(cg, &opcode, 1);
}

// MOV r/m64, imm32
void mov_reg_imm32(TCodeGen* cg, ERegister reg, int32_t imm) {
    // opcode: REX.W + C7 /0 imm32
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=000, R/M=reg_code)
    uint8_t modrm = 0xc0 + (reg & 0x7);
    uint8_t opcode[] = {0x48, 0xc7, modrm}; 
    AppendCode(cg, opcode, 3);
    AppendCode(cg, (uint8_t*)&imm, 4);
}

// MOV r64, r/m64
void mov_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 8B /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = 0xc0 + ((dst & 0x7) << 3) + (src & 0x7);
    uint8_t opcode[] = {0x48, 0x8b, modrm};
    AppendCode(cg, opcode, 3);
}

// MOV r64, [rbp-offset]
void mov_reg_mem(TCodeGen* cg, ERegister reg, int32_t offset) { 
    // opcode: REX.W + 8B /r disp32
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=10, Reg=reg, R/M=101(rbp))
    uint8_t modrm = 0x85 + ((reg & 0x7) << 3);
    uint8_t opcode[] = {0x48, 0x8b, modrm};
    AppendCode(cg, opcode, 3);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// MOV [rbp-offset], r64
void mov_mem_reg(TCodeGen* cg, int32_t offset, ERegister reg) { 
    // opcode: REX.W + 89 /r disp32
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=10, Reg=reg, R/M=101(rbp))
    uint8_t modrm = 0x85 + ((reg & 0x7) << 3);
    uint8_t opcode[] = {0x48, 0x89, modrm};
    AppendCode(cg, opcode, 3);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// ADD r/m64, r64
void add_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 01 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = 0xc0 + ((src & 0x7) << 3) + (dst & 0x7);
    uint8_t opcode[] = {0x48, 0x01, modrm};
    AppendCode(cg, opcode, 3);
}

// SUB r/m64, r64
void sub_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 29 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = 0xc0 + ((src & 0x7) << 3) + (dst & 0x7);
    uint8_t opcode[] = {0x48, 0x29, modrm};
    AppendCode(cg, opcode, 3);
}

// IMUL r64, r/m64
void imul_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 0F AF /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=dst, R/M=src)
    uint8_t modrm = 0xc0 + ((dst & 0x7) << 3) + (src & 0x7);
    uint8_t opcode[] = {0x48, 0x0f, 0xaf, modrm};
    AppendCode(cg, opcode, 4);
}

// IDIV r/m64
void idiv_reg(TCodeGen* cg, ERegister reg) {
    // opcode: REX.W + F7 /7
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=111, R/M=reg)
    uint8_t modrm = 0xf8 + (reg & 0x7);
    uint8_t opcode[] = {0x48, 0xf7, modrm};
    AppendCode(cg, opcode, 3);
}

// CMP r/m64, r64
void cmp_reg_reg(TCodeGen* cg, ERegister dst, ERegister src) {
    // opcode: REX.W + 39 /r
    // REX.W: 0x48 (64 bits)
    // ModR/M: (Mod=11, Reg=src, R/M=dst)
    uint8_t modrm = 0xc0 + ((src & 0x7) << 3) + (dst & 0x7);
    uint8_t opcode[] = {0x48, 0x39, modrm};
    AppendCode(cg, opcode, 3);
}

// JE rel32
void je_rel32(TCodeGen* cg, int32_t offset) {
    // opcode: 0F 84 imm32
    uint8_t opcode[] = {0x0f, 0x84};
    AppendCode(cg, opcode, 2);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// JMP rel32
void jmp_rel32(TCodeGen* cg, int32_t offset) {
    // opcode: E9 cd
    uint8_t opcode[] = {0xe9};
    AppendCode(cg, opcode, 1);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// CALL rel32
void call_rel32(TCodeGen* cg, int32_t offset) {
    // opcode: E8 cd
    uint8_t opcode[] = {0xe8};
    AppendCode(cg, opcode, 1);
    AppendCode(cg, (uint8_t*)&offset, 4);
}

// RET
void ret(TCodeGen* cg) {
    // opcode: С3
    uint8_t opcode[] = {0xc3};
    AppendCode(cg, opcode, 1);
}

// SYSCALL
void syscall(TCodeGen* cg) {
    // opcode: 0F 05
    uint8_t opcode[] = {0x0f, 0x05};
    AppendCode(cg, opcode, 2);
}
