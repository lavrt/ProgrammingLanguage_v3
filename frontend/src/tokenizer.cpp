#include "tokenizer.h"

#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unordered_set>
#include <fstream>
#include <filesystem>
#include <iostream>

#include "node.h"
#include "tree.h"
#include "debug.h"

// static ------------------------------------------------------------------------------------------

static const std::unordered_set<char> kAllowedSpecialChars = {
    '{', '}', '(', ')', ';', '+', '-', '*', '/', '<', '>', '=', '!'
};

// global ------------------------------------------------------------------------------------------

void tokenizer(std::vector<std::string>& tokens) {
    std::ifstream file(kNameOfFileWithCode);
    if (!file) {
        std::cerr << "The \"" << kNameOfFileWithCode << "\" file cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::filesystem::path filePath = kNameOfFileWithCode;
    size_t fileSize = std::filesystem::file_size(filePath);
    if (!fileSize) {
        std::cerr << "The \"" << kNameOfFileWithCode << "\" file is empty." << std::endl;
        exit(EXIT_FAILURE);
    }

    char* code = (char*)calloc(fileSize, sizeof(char));
    assert(code);

    file.read(code, fileSize);

    file.close();

    for (size_t i = 0; i < fileSize;) {
        while (i < fileSize && isspace(code[i])) {
            ++i;
        }

        std::string buffer;
        if (isalpha(code[i])) {
            while (i < fileSize && (isalnum(code[i]) || code[i] == '_')) {
                buffer += code[i++];
            }
            tokens.push_back(buffer);
        } else if (isdigit(code[i])) {
            while (i < fileSize && isdigit(code[i])) {
                buffer += code[i++];
            }
            tokens.push_back(buffer);
        } else if (i < fileSize && kAllowedSpecialChars.contains(code[i])) {
            buffer += code[i++];
            if (code[i] == '=') {
                buffer += code[i++];
            }
            tokens.push_back(buffer);
        } else if (code[i] == '#') {
            while (i < fileSize && code[i] != '\n') {
                ++i;
            }
        } else {
            std::cerr << "The token starts with an invalid character '" << code[i] << "'." << std::endl;
            exit(EXIT_FAILURE);
        }

        if (buffer == keyEnd) {
            break;
        }
    }

    free(code);
}
