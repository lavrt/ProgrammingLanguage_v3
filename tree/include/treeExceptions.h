#ifndef TREE_EXCEPTIONS_H
#define TREE_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace TreeExcept {

class BaseException : public std::runtime_error {
public:
    BaseException(const std::string& message)
        : std::runtime_error(message) {}
};

class FileException : public BaseException {
public:
    FileException(const std::string& message)
        : BaseException("File error: " + message) {}
};

class TreeException : public BaseException {
public:
    TreeException(const std::string& message)
        : BaseException("Tree error: " + message) {}
};

} // namespace TreeExcept

#endif // TREE_EXCEPTIONS_H
