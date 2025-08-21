#ifndef TREE_EXCEPTIONS_H
#define TREE_EXCEPTIONS_H

#include <stdexcept>
#include <string>

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

class TreeException : public BaseException {
public:
    explicit TreeException(const std::string& message)
        : BaseException("Tree error: " + message) {}
};

#endif // TREE_EXCEPTIONS_H
