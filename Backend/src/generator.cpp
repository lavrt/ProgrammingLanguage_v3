#include "generator.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "node.h"
#include "asmCommands.h"

void CodeGenCtor(TCodeGen* cg) {
    cg->capacity = kInitCapacityOfCodeArray;
    cg->code = (uint8_t*)calloc(cg->capacity, sizeof(char));
    assert(cg->code);
    cg->size = 0;
    cg->stackOffset = 0;
    cg->labelCount = 0;
    cg->varCount = 0;
    cg->vars = (TVariables*)calloc(kAdditionalCapacityOfNameTable, sizeof(TVariables));
    assert(cg->vars);
    cg->funcCount = 0;
    cg->funcs = (TFunctions*)calloc(kAdditionalCapacityOfNameTable, sizeof(TFunctions));
    assert(cg->funcs);
}

void CodeGenDtor(TCodeGen* cg) {
    free(cg->code);
    cg->code = NULL;
}

void AppendCode(TCodeGen* cg, const uint8_t* data, size_t len) {
    if (cg->size + len > cg->capacity) {
        cg->capacity *= 2;
        cg->code = (uint8_t*)realloc(cg->code, cg->capacity);
        assert(cg->code);
    }
    memcpy(cg->code + cg->size, data, len);
    cg->size += len;
}

int FindVar(TCodeGen* cg, const char* id) {
    for (size_t i = 0; i != cg->varCount; ++i) {
        if (!strcmp(cg->vars[i].id, id)) {
            return cg->vars[i].offset;
        }
    }
    return -1;
}

void AddVar(TCodeGen* cg, const char* id) {
    if (cg->varCount >= kAdditionalCapacityOfNameTable) {
        cg->vars = (TVariables*)realloc(
            cg->vars, (cg->varCount + kAdditionalCapacityOfNameTable) * sizeof(TVariables)
        );
        assert(cg->vars);
    }
    cg->vars[cg->varCount].id = strdup(id);
    cg->stackOffset += 8;
    cg->vars[cg->varCount].offset = cg->stackOffset;
    ++cg->varCount;
}

size_t FindFunc(TCodeGen* cg, const char* name) {
    for (int i = 0; i < cg->funcCount; i++) {
        if (!strcmp(cg->funcs[i].name, name)) {
            return cg->funcs[i].addr;
        }
    }
    return 0;
}

void AddFunc(TCodeGen* cg, const char* name) {
    if (cg->funcCount >= kAdditionalCapacityOfNameTable) {
        cg->funcs = (TFunctions*)realloc(
            cg->funcs, (cg->funcCount + kAdditionalCapacityOfNameTable) * sizeof(TFunctions)
        );
        assert(cg->funcs);
    }
    cg->funcs[cg->funcCount].name = strdup(name);
    cg->funcs[cg->funcCount].addr = cg->size;
    ++cg->funcCount;
}

void CodeGen(TCodeGen* cg, tNode* node) {
    switch (node->type) {
        case Number: {
            mov_reg_imm32(cg, REG_AX, atoi(node->value));
            break;
        }
        //...in process
        
        default:
            break;
    }
}