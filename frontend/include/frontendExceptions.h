#ifndef FRONTEND_EXCEPTIONS_H
#define FRONTEND_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace FrontendExcept {

class BaseException : public std::runtime_error {
public:
    explicit BaseException(const std::string& message)
        : std::runtime_error(message) {}
};

class FileException : public BaseException {
public:
    explicit FileException(const std::string& message) 
        : BaseException("File error: " + message) {}
};

class TokenizerException : public BaseException {
public:
    explicit TokenizerException(const std::string& message) 
        : BaseException("Tokenizer error: " + message) {}
};

class ParserException : public BaseException {
public:
    explicit ParserException(const std::string& message) 
        : BaseException("Parser error: " + message) {}
};

} // namespace FrontendExcept

#endif // FRONTEND_EXCEPTIONS_H
