#ifndef BACKEND_EXCEPTIONS_H
#define BACKEND_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace BackendExcept {

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

class CodeGeneratorException : public BaseException {
public:
    explicit CodeGeneratorException(const std::string& message)
        : BaseException("Code generator error: " + message) {}
};

} // namespace BackendExcept

#endif // BACKEND_EXCEPTIONS_H
