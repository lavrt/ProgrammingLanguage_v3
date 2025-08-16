#ifndef GENERATOR_H
#define GENERATOR_H

#include <unordered_map>
#include <span>
#include <vector>
#include <string>

#include "headers.h"
#include "node.h"

class ScopeManager {
private:
    std::vector<std::unordered_map<std::string, int>> symbolStack;

public:
    void EnterScope() {
        symbolStack.emplace_back();
    }

    void ExitScope() {
        symbolStack.pop_back();
    }

    void AddSymbol(const std::string& name, int offset) {
        symbolStack.back()[name] = offset;
    }

    int Lookup(const std::string& name) {
        for (auto it = symbolStack.rbegin(); it != symbolStack.rend(); ++it) {
            if (it->contains(name)) {
                return it->at(name);
            }
        }
        return -1;
    }
};

class TCodeGen {
public:
    std::vector<uint8_t> code;
    int stackOffset;
    ScopeManager vars; 
    int varCount;
    std::unordered_map<std::string, size_t> funcs;
    int localStackOffset;
    bool isLocal;
};

void CodeGenCtor(TCodeGen* cg);
void AppendCode(TCodeGen* cg, std::span<uint8_t> data);
void CodegenProgram(TCodeGen* cg, Node* program, Elf64_Ehdr* ehdr);
void AddFunc(TCodeGen* cg, const std::string& name);

#endif // GENERATOR_H
