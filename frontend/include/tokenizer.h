#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <vector>
#include <string>
#include <unordered_set>

class Tokenizer {
private:
    std::vector<std::string> tokens;

    inline static const std::unordered_set<char> kAllowedSpecialChars {
        '{', '}', '(', ')', ';', '+', '-', '*', '/', '<', '>', '=', '!', ','
    };

    void SplitIntoTokens(const std::string& data);

public:
    Tokenizer(const std::string& fn);
    
    std::vector<std::string> GetTokens() const {
        return tokens;
    }
};

const char* const kNameOfFileWithCode = "./examples/code.rt";

#endif // TOKENIZER_H
