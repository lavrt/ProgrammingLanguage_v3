#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <stdint.h>

#include "headers.h"
#include "node.h"

struct TVariables {
    std::string id;
    int offset;
};

struct TFunctions {
    std::string name;
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
    int localStackOffset;
    bool isLocal;
};

void CodeGenCtor(TCodeGen* cg);
void CodeGenDtor(TCodeGen* cg);
void AppendCode(TCodeGen* cg, const uint8_t* data, size_t len);
void CodegenProgram(TCodeGen* cg, Node* program, Elf64_Ehdr* ehdr);
void AddFunc(TCodeGen* cg, const std::string& name);

#endif // GENERATOR_H
