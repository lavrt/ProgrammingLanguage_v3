#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <stdint.h>

const size_t kInitCapacity = 4096;

struct TVariables {
    char* id;
    int offset;
};

struct TFunctions {
    char* name;
    size_t addr;
};

struct TCodeGen {
    uint8_t* code;
    size_t size;
    size_t capacity;
    int stackOffset;
    int labelCount;
    TVariables* vars; 
    int varCount;
    TFunctions* funcs;
    int funcCount;
};

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

void CodeGenCtor(TCodeGen* cg);
void CodeGenDtor(TCodeGen* cg);
void AppendCode(TCodeGen* cg, const uint8_t* data, size_t len);

void push_reg(TCodeGen* cg, ERegister reg);
void pop_reg(TCodeGen* cg, ERegister reg);
void mov_reg_imm32(TCodeGen* cg, ERegister reg, int32_t imm);
void mov_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void mov_reg_mem(TCodeGen* cg, ERegister reg, int32_t offset);
void mov_mem_reg(TCodeGen* cg, int32_t offset, ERegister reg);
void add_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void sub_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void imul_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void idiv_reg(TCodeGen* cg, ERegister reg);
void cmp_reg_reg(TCodeGen* cg, ERegister dst, ERegister src);
void je_rel32(TCodeGen* cg, int32_t offset);
void jmp_rel32(TCodeGen* cg, int32_t offset);
void call_rel32(TCodeGen* cg, int32_t offset);
void ret(TCodeGen* cg);

#endif // GENERATOR_H