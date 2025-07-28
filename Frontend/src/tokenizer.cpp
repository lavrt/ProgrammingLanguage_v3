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
    FILE* file = fopen(kNameOfFileWithCode, "r");
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
            while (i < fileSize && kAllowedSpecialChars.contains(code[i])) {
                buffer += code[i++];
            }
            if (!isKeyWord(buffer.c_str())) {
                fprintf(stderr, "Unknown operator '%s'", buffer.c_str());
                exit(1);
            }
            tokens.push_back(strdup(buffer.c_str())); 
        } else {
            fprintf(stderr, "The token starts with an invalid character '%c'.\n", code[i]);
            exit(1);
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

size_t getFileSize(FILE* file) {
    assert(file);

    long currentPos = ftell(file);
    fseek(file, 0, SEEK_END);
    size_t size = (size_t)ftell(file);
    fseek(file, currentPos, SEEK_SET);

    return size;
}

bool isKeyWord(const char* const word) {
         
         if (!strcmp(word, keyAdd)) return true;
    else if (!strcmp(word, keySub)) return true;
    else if (!strcmp(word, keyMul)) return true;
    else if (!strcmp(word, keyDiv)) return true;
    else if (!strcmp(word, keySemicolon)) return true;
    else if (!strcmp(word, keyEqual)) return true;
    else if (!strcmp(word, keyLeftParenthesis)) return true; // doesnt exist in the tree
    else if (!strcmp(word, keyRightParenthesis)) return true; // doesnt exist in the tree
    else if (!strcmp(word, keyLeftCurlyBracket)) return true; // doesnt exist in the tree
    else if (!strcmp(word, keyRightCurlyBracket)) return true; // doesnt exist in the tree

    else if (!strcmp(word, keyLess)) return true;
    else if (!strcmp(word, keyGreater)) return true;
    else if (!strcmp(word, keyIdentical)) return true;
    else if (!strcmp(word, keyLessOrEqual)) return true;
    else if (!strcmp(word, keyNotIdentical)) return true;
    else if (!strcmp(word, keyGreaterOrEqual)) return true;

    else if (!strcmp(word, keyIf)) return true;
    else if (!strcmp(word, keyWhile)) return true;
    else if (!strcmp(word, keyPrintAscii)) return true;
    else if (!strcmp(word, keyPrintInt)) return true;
    else if (!strcmp(word, keyReturn)) return true;
    else if (!strcmp(word, keyEnd)) return true; // doesnt exist in the tree
    else if (!strcmp(word, keyDef)) return true; // doesnt exist in the tree
    else if (!strcmp(word, keyCall)) return true; // doesnt exist in the tree

    else return false;
}
