#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <stdint.h>

#include "node.h"

const size_t kInitCapacityOfCodeArray = 4096;
const int kAdditionalCapacityOfNameTable = 16;

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
int FindVar(TCodeGen* cg, const char* id);
void AddVar(TCodeGen* cg, const char* id);
size_t FindFunc(TCodeGen* cg, const char* name);
void AddFunc(TCodeGen* cg, const char* name);
void CodeGenExpr(TCodeGen* cg, tNode* node);
void CodeGenStmt(TCodeGen* cg, tNode* node);
void CodegenProgram(TCodeGen* cg, tNode* program);

#endif // GENERATOR_H