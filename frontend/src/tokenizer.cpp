#include "tokenizer.h"

#include <fstream>
#include <iostream>

#include "node.h"
#include "exceptions.h"

Tokenizer::Tokenizer(const std::string& fn) {
    std::ifstream file(fn);
    if (!file) {
        throw FileException("Cannot open file: " + fn);
    }

    std::string data {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    file.close();

    if (data.empty()) {
        throw FileException("File is empty: " + fn);
    }

    SplitIntoTokens(data);
}

void Tokenizer::SplitIntoTokens(const std::string& data) {
    for (size_t i = 0; i < data.size(); ) {
        while (i < data.size() && isspace(data[i])) {
            ++i;
        }

        if (i >= data.size()) {
            break;
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
        } else if (i < data.size() && data[i] == '#') {
            while (i < data.size() && data[i] != '\n') {
                ++i;
            }
        } else {
            throw TokenizerException(
                "Invalid character '" + std::string(1, data[i]) +
                "' at position " + std::to_string(i)
            );
        }
    }
}
