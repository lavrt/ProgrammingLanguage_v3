#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "debug.h"

// static --------------------------------------------------------------------------------------------------------------

static std::unique_ptr<TNode> GetGrammar(const std::vector<std::string>& tokens);
static std::unique_ptr<TNode> GetIf(const std::vector<std::string>& tokens, size_t* pos);
static std::unique_ptr<TNode> GetDef(const std::vector<std::string>& tokens, size_t* pos);
static std::unique_ptr<TNode> GetWhile(const std::vector<std::string>& tokens, size_t* pos);
static std::unique_ptr<TNode> GetNumber(const std::vector<std::string>& tokens, size_t* pos);
static std::unique_ptr<TNode> GetVariable(const std::vector<std::string>& tokens, size_t* pos);
static std::unique_ptr<TNode> GetOperation(const std::vector<std::string>& tokens, size_t* pos);
static std::unique_ptr<TNode> GetExpression(const std::vector<std::string>& tokens, size_t* pos);
static std::unique_ptr<TNode> GetComparsion(const std::vector<std::string>& tokens, size_t* pos);
static std::unique_ptr<TNode> GetAssignment(const std::vector<std::string>& tokens, size_t* pos);
static std::unique_ptr<TNode> GetParentheses(const std::vector<std::string>& tokens, size_t* pos);
static std::unique_ptr<TNode> GetMultiplication(const std::vector<std::string>& tokens, size_t* pos);

[[noreturn]] static void syntaxError(int line);
static bool isKeyWord(const std::string& word);

// global --------------------------------------------------------------------------------------------------------------

TTree RunParser(const std::vector<std::string>& tokens) {
    std::unique_ptr<TNode> root = GetGrammar(tokens);
    TTree ast(std::move(root));
    
    return ast;
}

// static --------------------------------------------------------------------------------------------------------------

static std::unique_ptr<TNode> GetGrammar(const std::vector<std::string>& tokens) {
    size_t pos = 0;

    if (tokens[pos] == keyEnd) {
        return std::make_unique<TNode>(End, keyEnd);
    }

    std::unique_ptr<TNode> left = GetDef(tokens, &pos);
    if (tokens[pos++] != keySemicolon) {
        syntaxError(__LINE__);
    }
    while (tokens[pos] != keyEnd) {
        std::unique_ptr<TNode> right = GetDef(tokens, &pos);
        if (tokens[pos++] != keySemicolon) {
            syntaxError(__LINE__);
        }
        left = std::make_unique<TNode>(Semicolon, keySemicolon, std::move(left), std::move(right)); 
    }

    return left;
}

static std::unique_ptr<TNode> GetExpression(const std::vector<std::string>& tokens, size_t* pos) {
    std::unique_ptr<TNode> left = GetMultiplication(tokens, pos);

    while (tokens[*pos] == keyAdd || tokens[*pos] == keySub) {
        size_t op = *pos;
        (*pos)++;
        std::unique_ptr<TNode> right = GetMultiplication(tokens, pos); 
        left = std::make_unique<TNode>(kStringToNodeType.at(tokens[op]), tokens[op], std::move(left), std::move(right));
    }
    return left;
}

static std::unique_ptr<TNode> GetComparsion(const std::vector<std::string>& tokens, size_t* pos) {
    std::unique_ptr<TNode> left = GetExpression(tokens, pos);

    if (tokens[*pos] == keyLess || tokens[*pos] == keyLessOrEqual ||
        tokens[*pos] == keyGreater || tokens[*pos] == keyGreaterOrEqual ||
        tokens[*pos] == keyIdentical || tokens[*pos] == keyNotIdentical) 
    {
        size_t op = *pos;
        (*pos)++;
        std::unique_ptr<TNode> right = GetExpression(tokens, pos);
        left = std::make_unique<TNode>(kStringToNodeType.at(tokens[op]), tokens[op], std::move(left), std::move(right));
    }

    return left;
}

static std::unique_ptr<TNode> GetMultiplication(const std::vector<std::string>& tokens, size_t* pos) {
    std::unique_ptr<TNode> left = GetParentheses(tokens, pos);

    while (tokens[*pos] == keyMul || tokens[*pos] == keyDiv) {
        size_t op = *pos;
        (*pos)++;
        std::unique_ptr<TNode> right = GetParentheses(tokens, pos);
        left = std::make_unique<TNode>(kStringToNodeType.at(tokens[op]), tokens[op], std::move(left), std::move(right));
    }
    return left;
}

static std::unique_ptr<TNode> GetParentheses(const std::vector<std::string>& tokens, size_t* pos) {
    if (tokens[*pos] == keyLeftParenthesis) {
        (*pos)++;
        std::unique_ptr<TNode> node = GetComparsion(tokens, pos);
        if (tokens[*pos] != keyRightParenthesis) {
            syntaxError(__LINE__);
        }
        (*pos)++;
        return node;
    } else if (!isdigit(tokens[*pos][0])) {
        return GetVariable(tokens, pos);
    } else if (isdigit(tokens[*pos][0])) {
        return GetNumber(tokens, pos);
    } else {
        syntaxError(__LINE__);
    }
}

static std::unique_ptr<TNode> GetNumber(const std::vector<std::string>& tokens, size_t* pos) {
    return std::make_unique<TNode>(Number, tokens[(*pos)++]);
}

static std::unique_ptr<TNode> GetVariable(const std::vector<std::string>& tokens, size_t* pos) {
    return std::make_unique<TNode>(Identifier, tokens[(*pos)++]);
}

static std::unique_ptr<TNode> GetDef(const std::vector<std::string>& tokens, size_t* pos) {
    if (tokens[*pos] == keyDef) {
        (*pos)++;
        size_t nameIndex = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        std::unique_ptr<TNode> left = GetVariable(tokens, pos);
        TNode* node = left.get();
        while (tokens[*pos] == keySemicolon) {
            (*pos)++;
            node->SetLeft(std::make_unique<TNode>(Identifier, tokens[*pos]));
            node = node->GetLeft();
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        std::unique_ptr<TNode> right = GetOperation(tokens, pos);
        return std::make_unique<TNode>(Def, tokens[nameIndex], std::move(left), std::move(right));
    }
    return GetOperation(tokens, pos);
}

static std::unique_ptr<TNode> GetOperation(const std::vector<std::string>& tokens, size_t* pos) {
    if (tokens[*pos] == keyIf) {
        return GetIf(tokens, pos);
    } else if (tokens[*pos] == keyWhile) {
        return GetWhile(tokens, pos);
    } else if (tokens[*pos] == keyPrintAscii) {
        size_t op = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        std::unique_ptr<TNode> left = GetComparsion(tokens, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        return std::make_unique<TNode>(PrintAscii, keyPrintAscii, std::move(left), nullptr); 
    } else if (tokens[*pos] == keyPrintInt) {
        size_t op = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        std::unique_ptr<TNode> left = GetComparsion(tokens, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        return std::make_unique<TNode>(PrintInt, keyPrintInt, std::move(left), nullptr); 
    } else if (tokens[*pos] == keyReturn) {
        size_t op = *pos;
        (*pos)++;
        std::unique_ptr<TNode> left = GetComparsion(tokens, pos);
        return std::make_unique<TNode>(Return, keyReturn, std::move(left), nullptr);
    } else if (tokens[*pos] == keyLeftCurlyBracket) {
        (*pos)++;
        std::unique_ptr<TNode> left = GetOperation(tokens, pos);
        if (tokens[(*pos)++] != keySemicolon) {
            syntaxError(__LINE__);
        }
        while (tokens[*pos] != keyRightCurlyBracket) {
            std::unique_ptr<TNode> rightNode = GetOperation(tokens, pos);
            if (tokens[(*pos)++] != keySemicolon) {
                syntaxError(__LINE__);
            }
            left = std::make_unique<TNode>(Semicolon, keySemicolon, std::move(left), std::move(rightNode));
        }

        if (tokens[(*pos)++] != keyRightCurlyBracket) {
            syntaxError(__LINE__);
        }

        return left;
    } else if (!isKeyWord(tokens[*pos])) {
        return GetAssignment(tokens, pos);
    } else {
        syntaxError(__LINE__);
    }
}

static std::unique_ptr<TNode> GetIf(const std::vector<std::string>& tokens, size_t* pos) {
    size_t op = *pos;
    (*pos)++;
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    std::unique_ptr<TNode> left = GetComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    std::unique_ptr<TNode> right = GetOperation(tokens, pos);

    return std::make_unique<TNode>(If, keyIf, std::move(left), std::move(right));
}

static std::unique_ptr<TNode> GetWhile(const std::vector<std::string>& tokens, size_t* pos) {
    size_t op = *pos;
    (*pos)++;
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    std::unique_ptr<TNode> left = GetComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    std::unique_ptr<TNode> right = GetOperation(tokens, pos);

    return std::make_unique<TNode>(While, keyWhile, std::move(left), std::move(right));
}

static std::unique_ptr<TNode> GetAssignment(const std::vector<std::string>& tokens, size_t* pos) {
    std::unique_ptr<TNode> leftNode = GetVariable(tokens, pos);
    std::unique_ptr<TNode> rightNode = nullptr;

    if (tokens[*pos] != keyEqual) {
        syntaxError(__LINE__);
    }

    size_t op = *pos;
    (*pos)++;
    if (tokens[*pos] == keyCall) {
        (*pos)++;
        size_t nameIndex = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        std::unique_ptr<TNode> argNode = GetVariable(tokens, pos);
        // while (tokens[*pos] == keySemicolon) { // FIXME ?? todo many args
        //     (*pos)++;
        //     argNode->left = NewNode(Identifier, tokens[*pos], nullptr, nullptr);
        //     (*pos)++;
        // }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        rightNode = std::make_unique<TNode>(Calling, tokens[nameIndex], std::move(argNode), nullptr); 
    } else if (tokens[*pos] == keyReadInt) {
        size_t op1 = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        rightNode = std::make_unique<TNode>(ReadInt, keyReadInt);
    } else {
        rightNode = GetComparsion(tokens, pos);
    }
    return std::make_unique<TNode>(Equal, keyEqual, std::move(leftNode), std::move(rightNode));
}

static void syntaxError(int line) {
    fprintf(stderr, "Syntax error in %d\n", line);

    exit(EXIT_FAILURE);
}

static bool isKeyWord(const std::string& word) {
    return word == keyAdd ||
        word == keySub ||
        word == keyMul ||
        word == keyDiv ||
        word == keySemicolon ||
        word == keyEqual ||
        word == keyLeftParenthesis ||
        word == keyRightParenthesis ||
        word == keyLeftCurlyBracket ||
        word == keyRightCurlyBracket ||
        word == keyLess ||
        word == keyGreater ||
        word == keyIdentical ||
        word == keyLessOrEqual ||
        word == keyNotIdentical ||
        word == keyGreaterOrEqual ||
        word == keyIf ||
        word == keyWhile ||
        word == keyPrintAscii ||
        word == keyPrintInt ||
        word == keyReadInt ||
        word == keyReturn ||
        word == keyEnd ||
        word == keyDef ||
        word == keyCall;
}