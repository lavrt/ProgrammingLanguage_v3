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

std::vector<std::string> Tokenizer(std::string data) {
    std::vector<std::string> tokens;
    
    for (size_t i = 0; i < data.size();) {
        while (i < data.size() && isspace(data[i])) {
            ++i;
        }

        if (i == data.size()) {
            std::cerr << "The file does not contain the keyword \"" << keyEnd << "\"." << std::endl;
            exit(EXIT_FAILURE);
        }

        std::string buffer;
        if (isalpha(data[i])) {
            while (i < data.size() && (isalnum(data[i]) || data[i] == '_')) {
                buffer += data[i++];
            }
            tokens.push_back(buffer);
        } else if (isdigit(data[i])) {
            while (i < data.size() && isdigit(data[i])) {
                buffer += data[i++];
            }
            tokens.push_back(buffer);
        } else if (i < data.size() && kAllowedSpecialChars.contains(data[i])) {
            buffer += data[i++];
            if (data[i] == '=') {
                buffer += data[i++];
            }
            tokens.push_back(buffer);
        } else if (data[i] == '#') {
            while (i < data.size() && data[i] != '\n') {
                ++i;
            }
        } else {
            std::cerr << "The token starts with an invalid character '" << data[i] << "'." << i << std::endl;
            exit(EXIT_FAILURE);
        }

        if (buffer == keyEnd) {
            break;
        }
    }

    return tokens;
}
