#ifndef PARSER_H
#define PARSER_H

#include "tree.h"

#include <vector>
#include <memory>
#include <string>

class Parser {
private:
    std::vector<std::string> tokens;
    size_t pos = 0;

    std::unique_ptr<Node> GetGrammar();
    std::unique_ptr<Node> GetIf();
    std::unique_ptr<Node> GetDef();
    std::unique_ptr<Node> GetWhile();
    std::unique_ptr<Node> GetNumber();
    std::unique_ptr<Node> GetVariable();
    std::unique_ptr<Node> GetOperation();
    std::unique_ptr<Node> GetExpression();
    std::unique_ptr<Node> GetComparsion();
    std::unique_ptr<Node> GetAssignment();
    std::unique_ptr<Node> GetParentheses();
    std::unique_ptr<Node> GetMultiplication();
    std::unique_ptr<Node> GetCalling();

    [[noreturn]] void SyntaxError();

public:
    Parser(const std::vector<std::string>& t) : tokens(t) {}

    Tree Parse() {
        std::unique_ptr<Node> root = GetGrammar();
        Tree ast(std::move(root));
    
        return ast;
    }
};

#define CHECK_LEFT_PARENTHESIS \
    do { if (tokens[pos] != keyLeftParenthesis) SyntaxError(); } while(0);
#define CHECK_RIGHT_PARENTHESIS \
    do { if (tokens[pos] != keyRightParenthesis) SyntaxError(); } while(0);

#endif // PARSER_H
