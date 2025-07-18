#ifndef ASM_COMMANDS_H
#define ASM_COMMANDS_H

#include "generator.h"

enum ERegister {
    REG_AX = 0,  
    REG_CX = 1, 
    REG_DX = 2,  
    REG_BX = 3,  
    REG_SP = 4,  
    REG_BP = 5,  
    REG_SI = 6,  
    REG_DI = 7,  
};

void push_reg(TCodeGen* cg, ERegister reg);
void pop_reg(TCodeGen* cg, ERegister reg);
void mov_reg_imm32(TCodeGen* cg, ERegister reg, int32_t imm);
void mov_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void mov_reg_mem(TCodeGen* cg, ERegister reg, int32_t offset);
void mov_mem_reg(TCodeGen* cg, int32_t offset, ERegister reg);
void add_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void add_reg_imm32(TCodeGen* cg, ERegister reg, int32_t imm);
void sub_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void sub_reg_imm32(TCodeGen* cg, ERegister reg, int32_t imm);
void imul_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void idiv_reg(TCodeGen* cg, ERegister reg);
void cmp_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void cmp_reg_imm32(TCodeGen* cg, ERegister reg, int32_t imm);
void je_rel32(TCodeGen* cg, int32_t offset);
void jmp_rel32(TCodeGen* cg, int32_t offset);
void jge_rel32(TCodeGen* cg, int32_t offset);
void jne_rel32(TCodeGen* cg, int32_t offset);
void call_rel32(TCodeGen* cg, int32_t offset);
void xor_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void neg_reg(TCodeGen* cg, ERegister reg);
void inc_reg(TCodeGen* cg, ERegister reg);
void dec_reg(TCodeGen* cg, ERegister reg);
void ret(TCodeGen* cg);
void syscall(TCodeGen* cg);
void nop(TCodeGen* cg);

#endif // ASM_COMMANDS_H