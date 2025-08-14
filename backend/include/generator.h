#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include <stdint.h>
#include <unordered_map>

#include "headers.h"
#include "node.h"

struct TCodeGen {
    uint8_t* code;
    size_t size;
    size_t capacity;
    int stackOffset;
    std::unordered_map<std::string, int> vars; 
    int varCount;
    std::unordered_map<std::string, size_t> funcs;
    int localStackOffset;
    bool isLocal;
};

void CodeGenCtor(TCodeGen* cg);
void CodeGenDtor(TCodeGen* cg);
void AppendCode(TCodeGen* cg, const uint8_t* data, size_t len);
void CodegenProgram(TCodeGen* cg, Node* program, Elf64_Ehdr* ehdr);
void AddFunc(TCodeGen* cg, const std::string& name);

#endif // GENERATOR_H
