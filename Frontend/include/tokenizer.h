#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <stdio.h>
#include <vector>

#include "node.h"
#include "vector.h"

const int kInitialSizeOfTokenVector = 64;
const char* const kNameOfFileWithCode = "code.rt";

struct Token {
    NodeType type;
    char* value;
    Token* left;
    Token* right;
};

void tokenizer(std::vector<char*>& tokens);
size_t getFileSize(FILE* file);
bool isKeyWord(const char* const word);
void freeTokens(std::vector<char*>& tokens);

#endif // TOKENIZER_H
