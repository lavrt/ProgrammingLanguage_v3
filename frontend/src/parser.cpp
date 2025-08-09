#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "tree.h"
#include "debug.h"

// static --------------------------------------------------------------------------------------------------------------

static tNode* getGrammar(const std::vector<char*>& tokens);
static tNode* getIf(const std::vector<char*>& tokens, size_t* pos);
static tNode* getDef(const std::vector<char*>& tokens, size_t* pos);
static tNode* getWhile(const std::vector<char*>& tokens, size_t* pos);
static tNode* getNumber(const std::vector<char*>& tokens, size_t* pos);
static tNode* getVariable(const std::vector<char*>& tokens, size_t* pos);
static tNode* getOperation(const std::vector<char*>& tokens, size_t* pos);
static tNode* getExpression(const std::vector<char*>& tokens, size_t* pos);
static tNode* getComparsion(const std::vector<char*>& tokens, size_t* pos);
static tNode* getAssignment(const std::vector<char*>& tokens, size_t* pos);
static tNode* getParentheses(const std::vector<char*>& tokens, size_t* pos);
static tNode* getMultiplication(const std::vector<char*>& tokens, size_t* pos);

[[noreturn]] static void syntaxError(int line);
static bool isKeyWord(const char* const word);

// global --------------------------------------------------------------------------------------------------------------

tNode* runParser(const std::vector<char*>& tokens) {
    return getGrammar(tokens);
}

// static --------------------------------------------------------------------------------------------------------------

static tNode* getGrammar(const std::vector<char*>& tokens) {
    size_t pos = 0;

    tNode* leftNode = getDef(tokens, &pos);
    if (strcmp(tokens[pos++], keySemicolon)) {
        syntaxError(__LINE__);
    }
    while (strcmp(tokens[pos], keyEnd)) {
        tNode* rightNode = getDef(tokens, &pos);
        if (strcmp(tokens[pos], keySemicolon)) {
            syntaxError(__LINE__);
        }
        leftNode = newNode(Operation, tokens[pos++], leftNode, rightNode);
    }

    return leftNode;
}

static tNode* getExpression(const std::vector<char*>& tokens, size_t* pos) {
    tNode* leftNode = getMultiplication(tokens, pos);

    while (!strcmp(tokens[*pos], keyAdd) || !strcmp(tokens[*pos], keySub)) {
        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getMultiplication(tokens, pos);
        leftNode = newNode(Binary, tokens[op], leftNode, rightNode);
    }
    return leftNode;
}

static tNode* getComparsion(const std::vector<char*>& tokens, size_t* pos) {
    tNode* leftNode = getExpression(tokens, pos);

    if (!strcmp(tokens[*pos], keyGreater) || !strcmp(tokens[*pos], keyLess ) || !strcmp(tokens[*pos], keyIdentical) ||
        !strcmp(tokens[*pos], keyGreaterOrEqual) || !strcmp(tokens[*pos], keyLessOrEqual) || !strcmp(tokens[*pos], keyNotIdentical)) {

        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getExpression(tokens, pos);
        leftNode = newNode(Binary, tokens[op], leftNode, rightNode);
    }

    return leftNode;
}

static tNode* getMultiplication(const std::vector<char*>& tokens, size_t* pos) {
    tNode* leftNode = getParentheses(tokens, pos);

    while (!strcmp(tokens[*pos], keyMul) || !strcmp(tokens[*pos], keyDiv)) {
        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getParentheses(tokens, pos);
        leftNode = newNode(Binary, tokens[op], leftNode, rightNode);
    }
    return leftNode;
}

static tNode* getParentheses(const std::vector<char*>& tokens, size_t* pos) {
    if (!strcmp(tokens[*pos], keyLeftParenthesis)) {
        (*pos)++;
        tNode* node = getComparsion(tokens, pos);
        if (strcmp(tokens[*pos], keyRightParenthesis)) {
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

static tNode* getNumber(const std::vector<char*>& tokens, size_t* pos) {
    return newNode(Number, tokens[(*pos)++], nullptr, nullptr);
}

static tNode* getVariable(const std::vector<char*>& tokens, size_t* pos) {
    return newNode(Identifier, tokens[(*pos)++], nullptr, nullptr);
}

static tNode* getDef(const std::vector<char*>& tokens, size_t* pos) {
    if (!strcmp(tokens[*pos], keyDef)) {
        (*pos)++;
        char* name = tokens[*pos];
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* leftNode = getVariable(tokens, pos);
        tNode* node = leftNode;
        while (!strcmp(tokens[*pos], keySemicolon)) {
            (*pos)++;
            leftNode->left = newNode(Identifier, tokens[*pos], nullptr, nullptr);
            leftNode = leftNode->left;
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        tNode* rightNode = getOperation(tokens, pos);
        return newNode(Function, name, node, rightNode);
    }
    tNode* leftNode = getOperation(tokens, pos);
    return leftNode;
}

static tNode* getOperation(const std::vector<char*>& tokens, size_t* pos) {
    if (!strcmp(tokens[*pos], keyIf)) {
        return getIf(tokens, pos);
    } else if (!strcmp(tokens[*pos], keyWhile)) {
        return getWhile(tokens, pos);
    } else if (!strcmp(tokens[*pos], keyPrintAscii)) {
        size_t op = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* leftNode = getComparsion(tokens, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        return newNode(Operation, tokens[op], leftNode, nullptr); 
    } else if (!strcmp(tokens[*pos], keyPrintInt)) {
        size_t op = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* leftNode = getComparsion(tokens, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        return newNode(Operation, tokens[op], leftNode, nullptr);
    } else if (!strcmp(tokens[*pos], keyReturn)) {
        size_t op = *pos;
        (*pos)++;
        tNode* leftNode = getComparsion(tokens, pos);
        return newNode(Operation, tokens[op], leftNode, nullptr);
    } else if (!strcmp(tokens[*pos], keyLeftCurlyBracket)) {
        (*pos)++;
        tNode* leftNode = getOperation(tokens, pos);
        if (strcmp(tokens[(*pos)++], keySemicolon)) {
            syntaxError(__LINE__);
        }
        while (strcmp(tokens[*pos], keyRightCurlyBracket)) {
            tNode* rightNode = getOperation(tokens, pos);
            if (strcmp(tokens[(*pos)], keySemicolon)) {
                syntaxError(__LINE__);
            }
            leftNode = newNode(Operation, tokens[(*pos)++], leftNode, rightNode);
        }

        if (strcmp(tokens[(*pos)++], keyRightCurlyBracket)) {
            syntaxError(__LINE__);
        }

        return leftNode;
    } else if (!isKeyWord(tokens[*pos])) {
        return getAssignment(tokens, pos);
    } else {
        syntaxError(__LINE__);
    }
}

static tNode* getIf(const std::vector<char*>& tokens, size_t* pos) {
    size_t op = *pos;
    (*pos)++;
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    tNode* leftNode = getComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    tNode* rightNode = getOperation(tokens, pos);

    return newNode(Operation, tokens[op], leftNode, rightNode);
}

static tNode* getWhile(const std::vector<char*>& tokens, size_t* pos) {
    size_t op = *pos;
    (*pos)++;
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    tNode* leftNode = getComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    tNode* rightNode = getOperation(tokens, pos);

    return newNode(Operation, tokens[op], leftNode, rightNode);
}

static tNode* getAssignment(const std::vector<char*>& tokens, size_t* pos) {
    tNode* leftNode = getVariable(tokens, pos);
    tNode* rightNode = nullptr;
    if (strcmp(tokens[*pos], keyEqual)) {
        syntaxError(__LINE__);
    }
    size_t op = *pos;
    (*pos)++;
    if (!strcmp(tokens[*pos], keyCall)) {
        (*pos)++;
        char* name = tokens[*pos];
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* argNode = getVariable(tokens, pos);
        while (!strcmp(tokens[*pos], keySemicolon)) {
            (*pos)++;
            argNode->left = newNode(Identifier, tokens[*pos], nullptr, nullptr);
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        rightNode = newNode(Calling, name, argNode, nullptr); 
    } else if (!strcmp(tokens[*pos], keyReadInt)) {
        size_t op1 = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        rightNode = newNode(Operation, tokens[op1], nullptr, nullptr);
    } else {
        rightNode = getComparsion(tokens, pos);
    }
    return newNode(Operation, tokens[op], leftNode, rightNode);
}

static void syntaxError(int line) {
    fprintf(stderr, "Syntax error in %d\n", line);

    exit(EXIT_FAILURE);
}

static bool isKeyWord(const char* const word) {
         if (!strcmp(word, keyAdd)) return true;
    else if (!strcmp(word, keySub)) return true;
    else if (!strcmp(word, keyMul)) return true;
    else if (!strcmp(word, keyDiv)) return true;
    else if (!strcmp(word, keySemicolon)) return true;
    else if (!strcmp(word, keyEqual)) return true;
    else if (!strcmp(word, keyLeftParenthesis)) return true; // doesnt exist in the tree
    else if (!strcmp(word, keyRightParenthesis)) return true; // doesnt exist in the tree
    else if (!strcmp(word, keyLeftCurlyBracket)) return true; // doesnt exist in the tree
    else if (!strcmp(word, keyRightCurlyBracket)) return true; // doesnt exist in the tree

    else if (!strcmp(word, keyLess)) return true;
    else if (!strcmp(word, keyGreater)) return true;
    else if (!strcmp(word, keyIdentical)) return true;
    else if (!strcmp(word, keyLessOrEqual)) return true;
    else if (!strcmp(word, keyNotIdentical)) return true;
    else if (!strcmp(word, keyGreaterOrEqual)) return true;

    else if (!strcmp(word, keyIf)) return true;
    else if (!strcmp(word, keyWhile)) return true;
    else if (!strcmp(word, keyPrintAscii)) return true;
    else if (!strcmp(word, keyPrintInt)) return true;
    else if (!strcmp(word, keyReadInt)) return true;
    else if (!strcmp(word, keyReturn)) return true;
    else if (!strcmp(word, keyEnd)) return true; // doesnt exist in the tree
    else if (!strcmp(word, keyDef)) return true; // doesnt exist in the tree
    else if (!strcmp(word, keyCall)) return true; // doesnt exist in the tree

    else return false;
}