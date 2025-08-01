#include "tokenizer.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unordered_set>
#include <string>

#include "node.h"
#include "tree.h"
#include "debug.h"

// static ------------------------------------------------------------------------------------------

static const std::unordered_set<char> kAllowedSpecialChars = {
    '{', '}', '(', ')', ';', '+', '-', '*', '/', '<', '>', '=', '!'
};

// global ------------------------------------------------------------------------------------------

void tokenizer(std::vector<char*>& tokens) {
    FILE* file = fopen(kNameOfFileWithCode, "rb");
    assert(file);

    size_t fileSize = getFileSize(file);

    char* code = (char*)calloc(fileSize + 1, sizeof(char));
    assert(code);

    fread(code, sizeof(char), fileSize, file);

    fclose(file);

    for (size_t i = 0; i < fileSize;) {
        while (i < fileSize && isspace(code[i])) {
            ++i;
        }

        std::string buffer;
        if (isalpha(code[i])) {
            while (i < fileSize && (isalnum(code[i]) || code[i] == '_')) {
                buffer += code[i++];
            }
            tokens.push_back(strdup(buffer.c_str()));
        } else if (isdigit(code[i])) {
            while (i < fileSize && isdigit(code[i])) {
                buffer += code[i++];
            }
            tokens.push_back(strdup(buffer.c_str())); 
        } else if (kAllowedSpecialChars.contains(code[i])) {
            buffer += code[i++];
            if (code[i] == '=') {
                buffer += code[i++];
            }
            tokens.push_back(strdup(buffer.c_str())); 
        } else {
            fprintf(stderr, "The token starts with an invalid character '%c'.\n", code[i]);
            exit(EXIT_FAILURE);
        }

        if (buffer == keyEnd) {
            break;
        }
    }

    free(code);
}

void freeTokens(std::vector<char*>& tokens) {
    for (char* token : tokens) {
        free(token);
    }
}


