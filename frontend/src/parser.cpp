#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "tree.h"
#include "debug.h"

// static --------------------------------------------------------------------------------------------------------------

static tNode* getGrammar(const std::vector<std::string>& tokens);
static tNode* getIf(const std::vector<std::string>& tokens, size_t* pos);
static tNode* getDef(const std::vector<std::string>& tokens, size_t* pos);
static tNode* getWhile(const std::vector<std::string>& tokens, size_t* pos);
static tNode* getNumber(const std::vector<std::string>& tokens, size_t* pos);
static tNode* getVariable(const std::vector<std::string>& tokens, size_t* pos);
static tNode* getOperation(const std::vector<std::string>& tokens, size_t* pos);
static tNode* getExpression(const std::vector<std::string>& tokens, size_t* pos);
static tNode* getComparsion(const std::vector<std::string>& tokens, size_t* pos);
static tNode* getAssignment(const std::vector<std::string>& tokens, size_t* pos);
static tNode* getParentheses(const std::vector<std::string>& tokens, size_t* pos);
static tNode* getMultiplication(const std::vector<std::string>& tokens, size_t* pos);

[[noreturn]] static void syntaxError(int line);
static bool isKeyWord(const std::string& word);

// global --------------------------------------------------------------------------------------------------------------

tNode* runParser(const std::vector<std::string>& tokens) {
    return getGrammar(tokens);
}

// static --------------------------------------------------------------------------------------------------------------

static tNode* getGrammar(const std::vector<std::string>& tokens) {
    size_t pos = 0;

    tNode* leftNode = getDef(tokens, &pos);
    if (tokens[pos++] != keySemicolon) {
        syntaxError(__LINE__);
    }
    while (tokens[pos] != keyEnd) {
        tNode* rightNode = getDef(tokens, &pos);
        if (tokens[pos] != keySemicolon) {
            syntaxError(__LINE__);
        }
        leftNode = newNode(Semicolon, tokens[pos++], leftNode, rightNode);
    }

    return leftNode;
}

static tNode* getExpression(const std::vector<std::string>& tokens, size_t* pos) {
    tNode* leftNode = getMultiplication(tokens, pos);

    while (tokens[*pos] == keyAdd || tokens[*pos] == keySub) {
        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getMultiplication(tokens, pos); 
        leftNode = newNode(kStringToNodeType.at(tokens[op]), tokens[op], leftNode, rightNode);//NOTE
    }
    return leftNode;
}

static tNode* getComparsion(const std::vector<std::string>& tokens, size_t* pos) {
    tNode* leftNode = getExpression(tokens, pos);

    if (tokens[*pos] == keyLess ||
        tokens[*pos] == keyGreater || 
        tokens[*pos] == keyIdentical ||
        tokens[*pos] == keyLessOrEqual ||
        tokens[*pos] == keyGreaterOrEqual ||
        tokens[*pos] == keyNotIdentical) 
    {
        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getExpression(tokens, pos);
        leftNode = newNode(kStringToNodeType.at(tokens[op]), tokens[op], leftNode, rightNode);//NOTE
    }

    return leftNode;
}

static tNode* getMultiplication(const std::vector<std::string>& tokens, size_t* pos) {
    tNode* leftNode = getParentheses(tokens, pos);

    while (tokens[*pos] == keyMul || tokens[*pos] == keyDiv) {
        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getParentheses(tokens, pos);
        leftNode = newNode(kStringToNodeType.at(tokens[op]), tokens[op], leftNode, rightNode);//NOTE
    }
    return leftNode;
}

static tNode* getParentheses(const std::vector<std::string>& tokens, size_t* pos) {
    if (tokens[*pos] == keyLeftParenthesis) {
        (*pos)++;
        tNode* node = getComparsion(tokens, pos);
        if (tokens[*pos] != keyRightParenthesis) {
            syntaxError(__LINE__);
        }
        (*pos)++;
        return node;
    } else if (!isdigit(tokens[*pos][0])) {
        tNode* node = getVariable(tokens, pos);
        return node;
    } else if (isdigit(tokens[*pos][0])) {
        return getNumber(tokens, pos);
    } else assert(0);
}

static tNode* getNumber(const std::vector<std::string>& tokens, size_t* pos) {
    return newNode(Number, tokens[(*pos)++], nullptr, nullptr);
}

static tNode* getVariable(const std::vector<std::string>& tokens, size_t* pos) {
    return newNode(Identifier, tokens[(*pos)++], nullptr, nullptr);
}

static tNode* getDef(const std::vector<std::string>& tokens, size_t* pos) {
    if (tokens[*pos] == keyDef) {
        (*pos)++;
        std::string name = tokens[*pos];
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* leftNode = getVariable(tokens, pos);
        tNode* node = leftNode;
        while (tokens[*pos] == keySemicolon) {
            (*pos)++;
            leftNode->left = newNode(Identifier, tokens[*pos], nullptr, nullptr);
            leftNode = leftNode->left;
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        tNode* rightNode = getOperation(tokens, pos);
        return newNode(Def, name, node, rightNode);
    }
    tNode* leftNode = getOperation(tokens, pos);
    return leftNode;
}

static tNode* getOperation(const std::vector<std::string>& tokens, size_t* pos) {
    if (tokens[*pos] == keyIf) {
        return getIf(tokens, pos);
    } else if (tokens[*pos] == keyWhile) {
        return getWhile(tokens, pos);
    } else if (tokens[*pos] == keyPrintAscii) {
        size_t op = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* leftNode = getComparsion(tokens, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        return newNode(PrintAscii, tokens[op], leftNode, nullptr); 
    } else if (tokens[*pos] == keyPrintInt) {
        size_t op = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* leftNode = getComparsion(tokens, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        return newNode(PrintInt, tokens[op], leftNode, nullptr);
    } else if (tokens[*pos] == keyReturn) {
        size_t op = *pos;
        (*pos)++;
        tNode* leftNode = getComparsion(tokens, pos);
        return newNode(Return, tokens[op], leftNode, nullptr);
    } else if (tokens[*pos] == keyLeftCurlyBracket) {
        (*pos)++;
        tNode* leftNode = getOperation(tokens, pos);
        if (tokens[(*pos)++] != keySemicolon) {
            syntaxError(__LINE__);
        }
        while (tokens[*pos] != keyRightCurlyBracket) {
            tNode* rightNode = getOperation(tokens, pos);
            if (tokens[(*pos)] != keySemicolon) {
                syntaxError(__LINE__);
            }
            leftNode = newNode(Semicolon, tokens[(*pos)++], leftNode, rightNode);
        }

        if (tokens[(*pos)++] != keyRightCurlyBracket) {
            syntaxError(__LINE__);
        }

        return leftNode;
    } else if (!isKeyWord(tokens[*pos])) {
        return getAssignment(tokens, pos);
    } else {
        syntaxError(__LINE__);
    }
}

static tNode* getIf(const std::vector<std::string>& tokens, size_t* pos) {
    size_t op = *pos;
    (*pos)++;
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    tNode* leftNode = getComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    tNode* rightNode = getOperation(tokens, pos);

    return newNode(If, tokens[op], leftNode, rightNode);
}

static tNode* getWhile(const std::vector<std::string>& tokens, size_t* pos) {
    size_t op = *pos;
    (*pos)++;
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    tNode* leftNode = getComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    tNode* rightNode = getOperation(tokens, pos);

    return newNode(While, tokens[op], leftNode, rightNode);
}

static tNode* getAssignment(const std::vector<std::string>& tokens, size_t* pos) {
    tNode* leftNode = getVariable(tokens, pos);
    tNode* rightNode = nullptr;
    if (tokens[*pos] != keyEqual) {
        syntaxError(__LINE__);
    }
    size_t op = *pos;
    (*pos)++;
    if (tokens[*pos] == keyCall) {
        (*pos)++;
        std::string name = tokens[*pos];
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* argNode = getVariable(tokens, pos);
        while (tokens[*pos] == keySemicolon) {
            (*pos)++;
            argNode->left = newNode(Identifier, tokens[*pos], nullptr, nullptr);
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        rightNode = newNode(Calling, name, argNode, nullptr); 
    } else if (tokens[*pos] == keyReadInt) {
        size_t op1 = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        rightNode = newNode(ReadInt, tokens[op1], nullptr, nullptr);
    } else {
        rightNode = getComparsion(tokens, pos);
    }
    return newNode(Equal, tokens[op], leftNode, rightNode);
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