#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <stdint.h>

#include "headers.h"
#include "node.h"

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

void CodeGenCtor(TCodeGen* cg);
void CodeGenDtor(TCodeGen* cg);
void AppendCode(TCodeGen* cg, const uint8_t* data, size_t len);
void CodegenProgram(TCodeGen* cg, tNode* program, Elf64_Ehdr* ehdr);
void AddFunc(TCodeGen* cg, const char* name);

#define TEMP_JMP_OFFSET(cg) \
    uint32_t macro_initialSize_ = cg->size
    
#define SET_JMP_OFFSET \
    uint32_t macro_block_ = (uint32_t)cg->size - macro_initialSize_; \
    memcpy(cg->code + cg->size - macro_block_ - 4, (uint8_t*)&macro_block_, 4); \

#endif // GENERATOR_H