#include "generator.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "asmCommands.h"

// static ------------------------------------------------------------------------------------------

static Operations GetOperationType(const char* const word);

// global ------------------------------------------------------------------------------------------

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
    for (int i = 0; i != cg->varCount; ++i) {
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

void CodeGenExpr(TCodeGen* cg, tNode* node) {
    switch (node->type) {
        case Number: {
            mov_reg_imm32(cg, REG_AX, atoi(node->value));
            break;
        }
        case Identifier: {
            int offset = FindVar(cg, node->value);
            if (offset == -1) {
                fprintf(stderr, "Undefined variable\n");
                exit(1);
            }
            mov_reg_mem(cg, REG_AX, -offset);
            break;
        }
        case Binary: {
            CodeGenExpr(cg, node->left);
            push_reg(cg, REG_AX);
            CodeGenExpr(cg, node->right);
            pop_reg(cg, REG_BX);
            switch (GetOperationType(node->value)) {
                case Add: add_reg_reg(cg, REG_AX, REG_BX); break;
                case Sub: sub_reg_reg(cg, REG_AX, REG_BX); break;
                case Mul: imul_reg_reg(cg, REG_AX, REG_BX); break;
                case Div: {
                    uint8_t xor_rdx_rdx[] = {0x48, 0x31, 0xd2};
                    AppendCode(cg, xor_rdx_rdx, 3);
                    uint8_t xchg_rax_rbx[] = {0x48, 0x93};
                    AppendCode(cg, xchg_rax_rbx, 2);
                    idiv_reg(cg, REG_BX);
                    break;
                }
                case Greater: {
                    cmp_reg_reg(cg, REG_AX, REG_BX);
                    uint8_t setg_al[] = {0x0f, 0x9f, 0xc0}; // opcode: 0F 9F /0
                    AppendCode(cg, setg_al, 3);
                    uint8_t movzx_rax_al[] = {0x48, 0x0f, 0xb6, 0xc0};
                    AppendCode(cg, movzx_rax_al, 4);
                    break;
                }
                case Identical: {
                    cmp_reg_reg(cg, REG_AX, REG_BX);
                    uint8_t sete_al[] = {0x0f, 0x94, 0xc0}; // opcode: 0F 94 /0
                    AppendCode(cg, sete_al, 3);
                    uint8_t movzx_rax_al[] = {0x48, 0x0f, 0xb6, 0xc0};
                    AppendCode(cg, movzx_rax_al, 4);
                    break;
                }
                default: {
                    fprintf(stderr, "Unknown binary operator\n");
                    exit(1);
                }
            }
            break;
        }
        case Calling: {
            push_reg(cg, REG_DI);
            push_reg(cg, REG_SI);
            tNode* arg = node->left;
            if (arg) {
                CodeGenExpr(cg, arg);
                mov_reg_reg(cg, REG_DI, REG_AX);
                arg = arg->left;
                if (arg) {
                    CodeGenExpr(cg, arg);
                    mov_reg_reg(cg, REG_SI, REG_AX);
                }
            }
            size_t addr = FindFunc(cg, node->value);
            if (!addr) {
                fprintf(stderr, "Undefined function\n");
                exit(1);
            }
            call_rel32(cg, addr - (cg->size + 5)); // 5 is the size of the call_rel32
            pop_reg(cg, REG_SI);
            pop_reg(cg, REG_DI);
            break;
        }
        
        default: {
            fprintf(stderr, "Unknown expression type\n");
            exit(1);
        }
    }
}

void CodeGenStmt(TCodeGen* cg, tNode* node) {
    if (!node) {
        return;
    }
    switch (node->type) {
        case Function: {

            break;
        }
        case Operation: {
            switch (GetOperationType(node->value)) {
                case Semicolon: {
                    CodeGenStmt(cg, node->left);
                    CodeGenStmt(cg, node->right);
                    break;
                }
                case Equal: {
                    CodeGenExpr(cg, node->right);
                    int offset = FindVar(cg, node->left->value);
                    if (offset == -1) {
                        AddVar(cg, node->left->value);
                        offset = 
                    }
                }
                default:
            }
        }

        default: break;
    }
}

// static ------------------------------------------------------------------------------------------

static Operations GetOperationType(const char* const word) {
         if (!strcmp(word, "if"    )) return If;               
    else if (!strcmp(word, "+"     )) return Add;              
    else if (!strcmp(word, "-"     )) return Sub;              
    else if (!strcmp(word, "*"     )) return Mul;              
    else if (!strcmp(word, "/"     )) return Div;               
    else if (!strcmp(word, "sin"   )) return Sin;
    else if (!strcmp(word, "cos"   )) return Cos; 
    else if (!strcmp(word, "sqrt"  )) return Sqrt;
    else if (!strcmp(word, "<"     )) return Less;           
    else if (!strcmp(word, "while" )) return While;          
    else if (!strcmp(word, "="     )) return Equal;       
    else if (!strcmp(word, "print" )) return Print;   
    else if (!strcmp(word, "return")) return Return;
    else if (!strcmp(word, ">"     )) return Greater;       
    else if (!strcmp(word, ";"     )) return Semicolon;        
    else if (!strcmp(word, "=="    )) return Identical;        
    else if (!strcmp(word, "<="    )) return LessOrEqual;       
    else if (!strcmp(word, "!="    )) return NotIdentical;     
    else if (!strcmp(word, ">="    )) return GreaterOrEqual;    

    else return NoOperation;
}