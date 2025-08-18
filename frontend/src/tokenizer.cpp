#include "tokenizer.h"

#include <fstream>
#include <iostream>

#include "node.h"

Tokenizer::Tokenizer(const std::string& fn) {
    std::ifstream file(kNameOfFileWithCode);
    if (!file) {
        std::cerr << "The \"" << kNameOfFileWithCode << "\" file cannot be opened." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string data {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    file.close();

    if (data.empty()) {
        std::cerr << "The \"" << kNameOfFileWithCode << "\" file is empty or cannot be read." << std::endl;
        exit(EXIT_FAILURE);
    }

    SplitIntoTokens(data);
}

void Tokenizer::SplitIntoTokens(const std::string& data) {
    for (size_t i = 0; i < data.size();) {
        while (i < data.size() && isspace(data[i])) {
            ++i;
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
        } else if (i >= data.size()) {
            break;
        } else {
            std::cerr << "The token starts with an invalid character '" << data[i] << "'." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}
