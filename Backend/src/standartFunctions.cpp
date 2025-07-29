#include "standartFunctions.h"

#include <string.h>

#include "asmCommands.h"

// static ------------------------------------------------------------------------------------------

static void CreatePrintAscii(TCodeGen* cg);
static void CreatePrintInt(TCodeGen* cg);

// global ------------------------------------------------------------------------------------------

void CreateStandartFunctions(TCodeGen* cg, Elf64_Ehdr* ehdr) {
    nop(cg); // it is not allowed to place functions with size = 0

    CreatePrintAscii(cg);
    CreatePrintInt(cg);

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
