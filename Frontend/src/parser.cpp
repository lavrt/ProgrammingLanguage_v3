#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "vector.h"
#include "tree.h"
#include "debug.h"
#include "dsl.h"

// static --------------------------------------------------------------------------------------------------------------

static tNode* getGrammar(std::vector<char*>& tokens);
static tNode* getIf(std::vector<char*>& tokens, size_t* pos);
static tNode* getDef(std::vector<char*>& tokens, size_t* pos);
static tNode* getWhile(std::vector<char*>& tokens, size_t* pos);
static tNode* getNumber(std::vector<char*>& tokens, size_t* pos);
static tNode* getVariable(std::vector<char*>& tokens, size_t* pos);
static tNode* getOperation(std::vector<char*>& tokens, size_t* pos);
static tNode* getExpression(std::vector<char*>& tokens, size_t* pos);
static tNode* getComparsion(std::vector<char*>& tokens, size_t* pos);
static tNode* getAssignment(std::vector<char*>& tokens, size_t* pos);
static tNode* getParentheses(std::vector<char*>& tokens, size_t* pos);
static tNode* getMultiplication(std::vector<char*>& tokens, size_t* pos);

[[noreturn]] static void syntaxError(int line);

// global --------------------------------------------------------------------------------------------------------------

tNode* runParser(std::vector<char*>& tokens) {
    tNode* root = getGrammar(tokens);

    return root;
}

// static --------------------------------------------------------------------------------------------------------------

static tNode* getGrammar(std::vector<char*>& tokens) {
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

static tNode* getExpression(std::vector<char*>& tokens, size_t* pos) {
    tNode* leftNode = getMultiplication(tokens, pos);

    while (!strcmp(tokens[*pos], keyAdd) || !strcmp(tokens[*pos], keySub)) {
        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getMultiplication(tokens, pos);
        leftNode = newNode(Binary, tokens[op], leftNode, rightNode);
    }
    return leftNode;
}

static tNode* getComparsion(std::vector<char*>& tokens, size_t* pos) {
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

static tNode* getMultiplication(std::vector<char*>& tokens, size_t* pos) {
    tNode* leftNode = getParentheses(tokens, pos);

    while (!strcmp(tokens[*pos], keyMul) || !strcmp(tokens[*pos], keyDiv)) {
        size_t op = *pos;
        (*pos)++;
        tNode* rightNode = getParentheses(tokens, pos);
        leftNode = newNode(Binary, tokens[op], leftNode, rightNode);
    }
    return leftNode;
}

static tNode* getParentheses(std::vector<char*>& tokens, size_t* pos) {
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

static tNode* getNumber(std::vector<char*>& tokens, size_t* pos) {
    return newNode(Number, tokens[(*pos)++], nullptr, nullptr);
}

static tNode* getVariable(std::vector<char*>& tokens, size_t* pos) {
    return newNode(Identifier, tokens[(*pos)++], nullptr, nullptr);
}

static tNode* getDef(std::vector<char*>& tokens, size_t* pos) {
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

static tNode* getOperation(std::vector<char*>& tokens, size_t* pos) {
    if (!strcmp(tokens[*pos], keyIf)) {
        (*pos)++;
        tNode* node = getIf(tokens, pos);
        return node;
    } else if (!strcmp(tokens[*pos], keyWhile)) {
        (*pos)++;
        tNode* node = getWhile(tokens, pos);
        return node;
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
        (*pos)++;
        tNode* leftNode = getComparsion(tokens, pos);
        return newNode(Operation, keyReturn, leftNode, NULL);
    } else if (!strcmp(tokens[*pos], keyLeftCurlyBracket)) {
        (*pos)++;
        tNode* leftNode = getOperation(tokens, pos);
        if (strcmp(tokens[(*pos)++], keySemicolon)) {
            syntaxError(__LINE__);
        }
        while (strcmp(tokens[*pos], keyRightCurlyBracket)) {
            tNode* rightNode = getOperation(tokens, pos);
            if (strcmp(tokens[(*pos)++], keySemicolon)) {
                syntaxError(__LINE__);
            }
            leftNode = SEMICOLON(leftNode, rightNode);
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

static tNode* getIf(std::vector<char*>& tokens, size_t* pos) {
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    tNode* leftNode = getComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    tNode* rightNode = getOperation(tokens, pos);

    return IF(leftNode, rightNode);
}

static tNode* getWhile(std::vector<char*>& tokens, size_t* pos) {
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    tNode* leftNode = getComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    tNode* rightNode = getOperation(tokens, pos);

    return WHILE(leftNode, rightNode);
}

static tNode* getAssignment(std::vector<char*>& tokens, size_t* pos) {
    tNode* leftNode = getVariable(tokens, pos);
    tNode* rightNode = NULL;
    if (strcmp(tokens[*pos], keyEqual)) {
        syntaxError(__LINE__);
    }
    (*pos)++;
    if (!strcmp(tokens[*pos], keyCall)) {
        (*pos)++;
        char* name = tokens[*pos];
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        tNode* leftNode = getVariable(tokens, pos);
        while (!strcmp(tokens[*pos], keySemicolon)) {
            (*pos)++;
            leftNode->left = newNode(Identifier, tokens[*pos], NULL, NULL);
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        rightNode = newNode(Calling, name, leftNode, NULL); 
    } else {
        rightNode = getComparsion(tokens, pos);
    }
    return EQUAL(leftNode, rightNode);
}

static void syntaxError(int line) {
    fprintf(stderr, "Syntax error in %d\n", line);

    exit(EXIT_FAILURE);
}
