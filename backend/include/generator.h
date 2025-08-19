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

    int FindSymbol(const std::string& name) {
        for (auto stackIter = symbolStack.rbegin(); stackIter != symbolStack.rend(); ++stackIter) {
            if (auto mapIter = stackIter->find(name); mapIter != stackIter->end()) {
                return mapIter->second;
            }
        }
        return -1;
    }
};

class FunctionManager {
private:
    std::unordered_map<std::string, size_t> mp;

public:
    void AddFunction(const std::string& name, size_t offset) {
        mp[name] = offset;
    }

    size_t FindFunction(const std::string& name) {
        auto iter = mp.find(name);
        return iter == mp.end() ? 0 : iter->second;
    }
};

class CodeBuffer {
private:
    std::vector<uint8_t> vec;

public:
    void Append(std::span<uint8_t> data) {
        vec.insert(vec.end(), data.begin(), data.end());
    }

    void Append(int32_t num) {
        std::span<uint8_t> data {
            reinterpret_cast<uint8_t*>(&num),
            sizeof(num)
        };
        vec.insert(vec.end(), data.begin(), data.end());
    }

    void InsertNumber(int32_t num, size_t pos) {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(&num);
        std::copy(data, data + sizeof(num), vec.begin() + pos);
    }

    size_t GetSize() const {
        return vec.size();
    }

    const uint8_t* GetData() const {
        return vec.data();
    }
};

class CodeGen {
private:

public:
    CodeBuffer code;
    ScopeManager vars;
    FunctionManager funcs;
};

void CodegenProgram(CodeGen* cg, Node* program, Elf64_Ehdr* ehdr);

#endif // GENERATOR_H
