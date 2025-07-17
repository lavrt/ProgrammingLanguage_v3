#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <stdint.h>

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
void CodegenProgram(TCodeGen* cg, tNode* program);

#endif // GENERATOR_H