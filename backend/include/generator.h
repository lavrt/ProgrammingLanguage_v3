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
    int stackOffset = 0;
    const int kWordSize = 8;

public:
    void EnterScope() {
        symbolStack.emplace_back();
    }

    void ExitScope() {
        stackOffset -= symbolStack.back().size() * kWordSize;
        symbolStack.pop_back();
    }

    int AddSymbol(const std::string& name) {
        stackOffset += kWordSize;
        symbolStack.back()[name] = stackOffset;
        return stackOffset;
    }

    int Lookup(const std::string& name) {
        for (auto stackIter = symbolStack.rbegin(); stackIter != symbolStack.rend(); ++stackIter) {
            if (auto mapIter = stackIter->find(name); mapIter != stackIter->end()) {
                return mapIter->second;
            }
        }
        return -1;
    }
};

class CodeGen {
private:

public:
    std::vector<uint8_t> code;
    ScopeManager vars;
    std::unordered_map<std::string, size_t> funcs;

};

void AppendCode(CodeGen* cg, std::span<uint8_t> data);
void CodegenProgram(CodeGen* cg, Node* program, Elf64_Ehdr* ehdr);
void AddFunc(CodeGen* cg, const std::string& name);

#endif // GENERATOR_H
