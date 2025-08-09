#include "standardFunctions.h"

#include <string.h>

#include "asmCommands.h"

// static ------------------------------------------------------------------------------------------

static void CreatePrintAscii(TCodeGen* cg);
static void CreatePrintInt(TCodeGen* cg);
static void CreateReadInt(TCodeGen* cg);

// global ------------------------------------------------------------------------------------------

void CreateStandartFunctions(TCodeGen* cg, Elf64_Ehdr* ehdr) {
    nop(cg); // it is not allowed to place functions with cg->size = 0

    CreatePrintAscii(cg);
    CreatePrintInt(cg);
    CreateReadInt(cg);

    ehdr->e_entry += cg->size;
}

// static ------------------------------------------------------------------------------------------

static void CreatePrintAscii(TCodeGen* cg) {
    AddFunc(cg, keyPrintAscii); 

    push_reg(cg, REG_BP);
    mov_reg_reg(cg, REG_BP, REG_SP);
    sub_reg_imm32(cg, REG_SP, 8);

    push_reg(cg, REG_DI);

    mov_reg_imm32(cg, REG_AX, 1);
    mov_reg_imm32(cg, REG_DI, 1);
    mov_reg_reg(cg, REG_SI, REG_SP);
    mov_reg_imm32(cg, REG_DX, 8);

    syscall(cg);

    mov_reg_reg(cg, REG_SP, REG_BP);
    pop_reg(cg, REG_BP);
    ret(cg);
}

static void CreatePrintInt(TCodeGen* cg) {
    AddFunc(cg, keyPrintInt);

    push_reg(cg, REG_BX);

    push_reg(cg, REG_BP);
    mov_reg_reg(cg, REG_BP, REG_SP);

    xor_reg_reg(cg, REG_CX, REG_CX);
    mov_reg_reg(cg, REG_AX, REG_DI);

    cmp_reg_imm32(cg, REG_AX, 0); 
    int32_t jmpPos_1 = (int32_t)cg->size;
    jge_rel32(cg, 0);

    neg_reg(cg, REG_AX); 
                                   
    push_reg(cg, REG_AX);
    push_reg(cg, REG_CX);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_SI);
    push_reg(cg, REG_DI);

    push_imm32(cg, '-');

    mov_reg_imm32(cg, REG_AX, 1);
    mov_reg_imm32(cg, REG_DI, 1);
    mov_reg_reg(cg, REG_SI, REG_SP);
    mov_reg_imm32(cg, REG_DX, 8);

    syscall(cg);

    add_reg_imm32(cg, REG_SP, 8);

    pop_reg(cg, REG_DI);
    pop_reg(cg, REG_SI);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_AX);

    int32_t jmpTarget_1 = (int32_t)cg->size;
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    memcpy(cg->code + jmpPos_1 + 2, (uint8_t*)&jmpOffset_1, 4);

    int32_t jmpTarget_2 = (int32_t)cg->size;

    xor_reg_reg(cg, REG_DX, REG_DX);
    mov_reg_imm32(cg, REG_BX, 10);
    idiv_reg(cg, REG_BX);
    push_reg(cg, REG_DX);
    inc_reg(cg, REG_CX);

    cmp_reg_imm32(cg, REG_AX, 0);
    int32_t jmpPos_2 = (int32_t)cg->size;
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 6);
    jne_rel32(cg, jmpOffset_2);

    int32_t jmpTarget_4 = (int32_t)cg->size;

    cmp_reg_imm32(cg, REG_CX, 0);
    int32_t jmpPos_3 = (int32_t)cg->size;
    je_rel32(cg, 0);

    pop_reg(cg, REG_DI);
    add_reg_imm32(cg, REG_DI, '0');

    push_reg(cg, REG_AX);
    push_reg(cg, REG_CX);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_SI);

    push_reg(cg, REG_DI);

    mov_reg_imm32(cg, REG_AX, 1);
    mov_reg_imm32(cg, REG_DI, 1);
    mov_reg_reg(cg, REG_SI, REG_SP);
    mov_reg_imm32(cg, REG_DX, 8);

    syscall(cg);

    pop_reg(cg, REG_DI);

    pop_reg(cg, REG_SI);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_AX);

    dec_reg(cg, REG_CX);

    int32_t jmpPos_4 = (int32_t)cg->size;
    int32_t jmpOffset_4 = jmpTarget_4 - (jmpPos_4 + 5);
    jmp_rel32(cg, jmpOffset_4); 

    int32_t jmpTarget_3 = (int32_t)cg->size;
    int32_t jmpOffset_3 = jmpTarget_3 - (jmpPos_3 + 6);
    memcpy(cg->code + jmpPos_3 + 2, (uint8_t*)&jmpOffset_3, 4);
                                   
    push_reg(cg, REG_AX);
    push_reg(cg, REG_CX);
    push_reg(cg, REG_DX);
    push_reg(cg, REG_SI);
    push_reg(cg, REG_DI);

    push_imm32(cg, '\n');

    mov_reg_imm32(cg, REG_AX, 1);
    mov_reg_imm32(cg, REG_DI, 1);
    mov_reg_reg(cg, REG_SI, REG_SP);
    mov_reg_imm32(cg, REG_DX, 8);

    syscall(cg);

    add_reg_imm32(cg, REG_SP, 8);
    
    pop_reg(cg, REG_DI);
    pop_reg(cg, REG_SI);
    pop_reg(cg, REG_DX);
    pop_reg(cg, REG_CX);
    pop_reg(cg, REG_AX);

    mov_reg_reg(cg, REG_SP, REG_BP);
    pop_reg(cg, REG_BP);

    pop_reg(cg, REG_BX);

    ret(cg);
}

static void CreateReadInt(TCodeGen* cg) {
    AddFunc(cg, keyReadInt);

    push_reg(cg, REG_BX);

    push_reg(cg, REG_BP);
    mov_reg_reg(cg, REG_BP, REG_SP);

    sub_reg_imm32(cg, REG_SP, 32);

    mov_reg_imm32(cg, REG_AX, 0);
    mov_reg_imm32(cg, REG_DI, 0);
    mov_reg_reg(cg, REG_SI, REG_SP);
    mov_reg_imm32(cg, REG_DX, 32);

    syscall(cg);

    mov_reg_reg(cg, REG_CX, REG_AX);
    xor_reg_reg(cg, REG_AX, REG_AX);
    mov_reg_imm32(cg, REG_DI, 1);
    mov_reg_reg(cg, REG_BX, REG_SP);

    int32_t jmpTarget_1 = (int32_t)cg->size;
    int32_t jmpTarget_2 = (int32_t)cg->size;
    int32_t jmpTarget_3 = (int32_t)cg->size;
    int32_t jmpTarget_4 = (int32_t)cg->size;

    cmp_reg_imm32(cg, REG_CX, 0);
    int32_t jmpPos_7 = (int32_t)cg->size;
    je_rel32(cg, 0);

    mov_reg8_mem(cg, REG_DX, REG_BX);
    inc_reg(cg, REG_BX);
    dec_reg(cg, REG_CX);

    cmp_reg_imm32(cg, REG_DX, ' ');
    int32_t jmpPos_1 = (int32_t)cg->size;
    int32_t jmpOffset_1 = jmpTarget_1 - (jmpPos_1 + 6);
    je_rel32(cg, jmpOffset_1);

    cmp_reg_imm32(cg, REG_DX, '\n');
    int32_t jmpPos_2 = (int32_t)cg->size;
    int32_t jmpOffset_2 = jmpTarget_2 - (jmpPos_2 + 6);
    je_rel32(cg, jmpOffset_2);

    cmp_reg_imm32(cg, REG_DX, '-');
    int32_t jmpPos_5 = (int32_t)cg->size;
    jne_rel32(cg, 0);

    mov_reg_imm32(cg, REG_DI, -1);

    int32_t jmpPos_3 = (int32_t)cg->size;
    int32_t jmpOffset_3 = jmpTarget_3 - (jmpPos_3 + 5);
    jmp_rel32(cg, jmpOffset_3);

    int32_t jmpTarget_5 = (int32_t)cg->size;
    int32_t jmpOffset_5 = jmpTarget_5 - (jmpPos_5 + 6);
    memcpy(cg->code + jmpPos_5 + 2, (uint8_t*)&jmpOffset_5, 4);

    cmp_reg_imm32(cg, REG_DX, '+');
    int32_t jmpPos_4 = (int32_t)cg->size;
    int32_t jmpOffset_4 = jmpTarget_4 - (jmpPos_4 + 6);
    je_rel32(cg, jmpOffset_4);

    mov_reg_imm32(cg, REG_SI, 10);

    int32_t jmpTarget_6 = (int32_t)cg->size;

    cmp_reg_imm32(cg, REG_CX, 0);
    int32_t jmpPos_8 = (int32_t)cg->size;
    je_rel32(cg, 0);
    
    sub_reg_imm32(cg, REG_DX, '0');

    cmp_reg_imm32(cg, REG_DX, 0);
    int32_t jmpPos_9 = (int32_t)cg->size;
    jl_rel32(cg, 0);

    cmp_reg_imm32(cg, REG_DX, 9);
    int32_t jmpPos_10 = (int32_t)cg->size;
    jg_rel32(cg, 0);

    imul_reg_reg(cg, REG_AX, REG_SI);
    add_reg_reg(cg, REG_AX, REG_DX);

    mov_reg8_mem(cg, REG_DX, REG_BX);
    inc_reg(cg, REG_BX);
    dec_reg(cg, REG_CX);

    int32_t jmpPos_6 = (int32_t)cg->size;
    int32_t jmpOffset_6 = jmpTarget_6 - (jmpPos_6 + 5);
    jmp_rel32(cg, jmpOffset_6);

    int32_t jmpTarget_7 = (int32_t)cg->size;
    int32_t jmpOffset_7 = jmpTarget_7 - (jmpPos_7 + 6);
    memcpy(cg->code + jmpPos_7 + 2, (uint8_t*)&jmpOffset_7, 4);

    int32_t jmpTarget_8 = (int32_t)cg->size;
    int32_t jmpOffset_8 = jmpTarget_8 - (jmpPos_8 + 6);
    memcpy(cg->code + jmpPos_8 + 2, (uint8_t*)&jmpOffset_8, 4);

    int32_t jmpTarget_9 = (int32_t)cg->size;
    int32_t jmpOffset_9 = jmpTarget_9 - (jmpPos_9 + 6);
    memcpy(cg->code + jmpPos_9 + 2, (uint8_t*)&jmpOffset_9, 4);
    
    int32_t jmpTarget_10 = (int32_t)cg->size;
    int32_t jmpOffset_10 = jmpTarget_10 - (jmpPos_10 + 6);
    memcpy(cg->code + jmpPos_10 + 2, (uint8_t*)&jmpOffset_10, 4);

    imul_reg_reg(cg, REG_AX, REG_DI);

    mov_reg_reg(cg, REG_SP, REG_BP);
    pop_reg(cg, REG_BP);

    pop_reg(cg, REG_BX);

    ret(cg);
}
