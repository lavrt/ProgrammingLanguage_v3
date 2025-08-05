#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <vector>

const char* const kNameOfFileWithCode = "./examples/code.rt";

void tokenizer(std::vector<char*>& tokens);
void freeTokens(std::vector<char*>& tokens);

#endif // TOKENIZER_H
