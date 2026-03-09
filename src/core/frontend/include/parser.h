#ifndef PARSER_H
#define PARSER_H

#include "tree.hpp"

#include <vector>
#include <memory>
#include <string>

class Parser {
private:
    Tree ast;

    std::vector<std::string> tokens;
    size_t pos = 0;

    Node* GetGrammar();
    Node* GetIf();
    Node* GetDef();
    Node* GetWhile();
    Node* GetNumber();
    Node* GetVariable();
    Node* GetOperation();
    Node* GetExpression();
    Node* GetComparsion();
    Node* GetAssignment();
    Node* GetParentheses();
    Node* GetMultiplication();
    Node* GetCalling();

    [[noreturn]] void SyntaxError();

public:
    Parser(const std::vector<std::string>& t) : tokens(t) {}

    Tree Parse() {
        ast.SetRoot(GetGrammar());
        return std::move(ast);
    }
};

#define CHECK_LEFT_PARENTHESIS \
    do { if (tokens[pos] != keyLeftParenthesis) SyntaxError(); } while(0);
#define CHECK_RIGHT_PARENTHESIS \
    do { if (tokens[pos] != keyRightParenthesis) SyntaxError(); } while(0);

#endif // PARSER_H
