#ifndef GENERATOR_H
#define GENERATOR_H

#include <unordered_map>
#include <span>
#include <vector>
#include <string>
#include <optional>

#include "asmCommands.h"
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

    std::optional<int> FindSymbol(const std::string& name) const {
        for (auto stackIter = symbolStack.rbegin(); stackIter != symbolStack.rend(); ++stackIter) {
            if (auto mapIter = stackIter->find(name); mapIter != stackIter->end()) {
                return mapIter->second;
            }
        }
        return std::nullopt;
    }
};

class FunctionManager {
private:
    std::unordered_map<std::string, size_t> mp;

public:
    void AddFunction(const std::string& name, size_t offset) {
        mp[name] = offset;
    }

    std::optional<size_t> FindFunction(const std::string& name) const noexcept {
        auto iter = mp.find(name);
        return iter == mp.end() ? std::nullopt : std::make_optional(iter->second);
    }
};

class CodeGen {
private:
    x86_64 asmGen;
    ScopeManager vars;
    FunctionManager funcs;

    void CreateElfHeader(Elf64_Ehdr* ehdr);
    void CreateProgramHeader(Elf64_Phdr* phdr, uint64_t filesz);

    void CreateStandartFunctions();
    void CreatePrintAscii();
    void CreatePrintInt();
    void CreateReadInt();

    void CodeGenExpr(Node* node);
    void CodeGenStmt(Node* node);

    void EmitNumber(Node* node);
    void EmitIdentifier(Node* node);
    void EmitCallInt(Node* node);
    void EmitReadInt();
    void EmitAdd(Node* node);
    void EmitSub(Node* node);
    void EmitMul(Node* node);
    void EmitDiv(Node* node);
    void EmitGreater(Node* node);
    void EmitGreaterOrEqual(Node* node);
    void EmitLess(Node* node);
    void EmitLessOrEqual(Node* node);
    void EmitIdentical(Node* node);
    void EmitNotIdentical(Node* node);
    void EmitDef(Node* node);
    void EmitSemicolon(Node* node);
    void EmitEqual(Node* node);
    void EmitPrintAscii(Node* node);
    void EmitPrintInt(Node* node);
    void EmitIf(Node* node);
    void EmitWhile(Node* node);
    void EmitReturn(Node* node);
    void EmitCallVoid(Node* node);

public:
    void GenerateProgram(Node* program, const std::string& fileName);
};

#endif // GENERATOR_H
