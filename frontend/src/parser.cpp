#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "tree.h"
#include "debug.h"

// static --------------------------------------------------------------------------------------------------------------

static TNode* GetGrammar(const std::vector<std::string>& tokens);
static TNode* GetIf(const std::vector<std::string>& tokens, size_t* pos);
static TNode* GetDef(const std::vector<std::string>& tokens, size_t* pos);
static TNode* GetWhile(const std::vector<std::string>& tokens, size_t* pos);
static TNode* GetNumber(const std::vector<std::string>& tokens, size_t* pos);
static TNode* GetVariable(const std::vector<std::string>& tokens, size_t* pos);
static TNode* GetOperation(const std::vector<std::string>& tokens, size_t* pos);
static TNode* GetExpression(const std::vector<std::string>& tokens, size_t* pos);
static TNode* GetComparsion(const std::vector<std::string>& tokens, size_t* pos);
static TNode* GetAssignment(const std::vector<std::string>& tokens, size_t* pos);
static TNode* GetParentheses(const std::vector<std::string>& tokens, size_t* pos);
static TNode* GetMultiplication(const std::vector<std::string>& tokens, size_t* pos);

[[noreturn]] static void syntaxError(int line);
static bool isKeyWord(const std::string& word);

// global --------------------------------------------------------------------------------------------------------------

TNode* RunParser(const std::vector<std::string>& tokens) {
    return GetGrammar(tokens);
}

// static --------------------------------------------------------------------------------------------------------------

static TNode* GetGrammar(const std::vector<std::string>& tokens) {
    size_t pos = 0;

    if (tokens[pos] == keyEnd) {
        return NewNode(End, tokens[pos], nullptr, nullptr);
    }

    TNode* leftNode = GetDef(tokens, &pos);
    if (tokens[pos++] != keySemicolon) {
        syntaxError(__LINE__);
    }
    while (tokens[pos] != keyEnd) {
        TNode* rightNode = GetDef(tokens, &pos);
        if (tokens[pos] != keySemicolon) {
            syntaxError(__LINE__);
        }
        leftNode = NewNode(Semicolon, tokens[pos++], leftNode, rightNode);
    }

    return leftNode;
}

static TNode* GetExpression(const std::vector<std::string>& tokens, size_t* pos) {
    TNode* leftNode = GetMultiplication(tokens, pos);

    while (tokens[*pos] == keyAdd || tokens[*pos] == keySub) {
        size_t op = *pos;
        (*pos)++;
        TNode* rightNode = GetMultiplication(tokens, pos); 
        leftNode = NewNode(kStringToNodeType.at(tokens[op]), tokens[op], leftNode, rightNode);
    }
    return leftNode;
}

static TNode* GetComparsion(const std::vector<std::string>& tokens, size_t* pos) {
    TNode* leftNode = GetExpression(tokens, pos);

    if (tokens[*pos] == keyLess ||
        tokens[*pos] == keyGreater || 
        tokens[*pos] == keyIdentical ||
        tokens[*pos] == keyLessOrEqual ||
        tokens[*pos] == keyGreaterOrEqual ||
        tokens[*pos] == keyNotIdentical) 
    {
        size_t op = *pos;
        (*pos)++;
        TNode* rightNode = GetExpression(tokens, pos);
        leftNode = NewNode(kStringToNodeType.at(tokens[op]), tokens[op], leftNode, rightNode);
    }

    return leftNode;
}

static TNode* GetMultiplication(const std::vector<std::string>& tokens, size_t* pos) {
    TNode* leftNode = GetParentheses(tokens, pos);

    while (tokens[*pos] == keyMul || tokens[*pos] == keyDiv) {
        size_t op = *pos;
        (*pos)++;
        TNode* rightNode = GetParentheses(tokens, pos);
        leftNode = NewNode(kStringToNodeType.at(tokens[op]), tokens[op], leftNode, rightNode);
    }
    return leftNode;
}

static TNode* GetParentheses(const std::vector<std::string>& tokens, size_t* pos) {
    if (tokens[*pos] == keyLeftParenthesis) {
        (*pos)++;
        TNode* node = GetComparsion(tokens, pos);
        if (tokens[*pos] != keyRightParenthesis) {
            syntaxError(__LINE__);
        }
        (*pos)++;
        return node;
    } else if (!isdigit(tokens[*pos][0])) {
        TNode* node = GetVariable(tokens, pos);
        return node;
    } else if (isdigit(tokens[*pos][0])) {
        return GetNumber(tokens, pos);
    } else assert(0);
}

static TNode* GetNumber(const std::vector<std::string>& tokens, size_t* pos) {
    return NewNode(Number, tokens[(*pos)++], nullptr, nullptr);
}

static TNode* GetVariable(const std::vector<std::string>& tokens, size_t* pos) {
    return NewNode(Identifier, tokens[(*pos)++], nullptr, nullptr);
}

static TNode* GetDef(const std::vector<std::string>& tokens, size_t* pos) {
    if (tokens[*pos] == keyDef) {
        (*pos)++;
        const std::string* name = &tokens[*pos];
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        TNode* leftNode = GetVariable(tokens, pos);
        TNode* node = leftNode;
        while (tokens[*pos] == keySemicolon) {
            (*pos)++;
            leftNode->left = NewNode(Identifier, tokens[*pos], nullptr, nullptr);
            leftNode = leftNode->left;
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        TNode* rightNode = GetOperation(tokens, pos);
        return NewNode(Def, *name, node, rightNode);
    }
    TNode* leftNode = GetOperation(tokens, pos);
    return leftNode;
}

static TNode* GetOperation(const std::vector<std::string>& tokens, size_t* pos) {
    if (tokens[*pos] == keyIf) {
        return GetIf(tokens, pos);
    } else if (tokens[*pos] == keyWhile) {
        return GetWhile(tokens, pos);
    } else if (tokens[*pos] == keyPrintAscii) {
        size_t op = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        TNode* leftNode = GetComparsion(tokens, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        return NewNode(PrintAscii, tokens[op], leftNode, nullptr); 
    } else if (tokens[*pos] == keyPrintInt) {
        size_t op = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        TNode* leftNode = GetComparsion(tokens, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        return NewNode(PrintInt, tokens[op], leftNode, nullptr);
    } else if (tokens[*pos] == keyReturn) {
        size_t op = *pos;
        (*pos)++;
        TNode* leftNode = GetComparsion(tokens, pos);
        return NewNode(Return, tokens[op], leftNode, nullptr);
    } else if (tokens[*pos] == keyLeftCurlyBracket) {
        (*pos)++;
        TNode* leftNode = GetOperation(tokens, pos);
        if (tokens[(*pos)++] != keySemicolon) {
            syntaxError(__LINE__);
        }
        while (tokens[*pos] != keyRightCurlyBracket) {
            TNode* rightNode = GetOperation(tokens, pos);
            if (tokens[(*pos)] != keySemicolon) {
                syntaxError(__LINE__);
            }
            leftNode = NewNode(Semicolon, tokens[(*pos)++], leftNode, rightNode);
        }

        if (tokens[(*pos)++] != keyRightCurlyBracket) {
            syntaxError(__LINE__);
        }

        return leftNode;
    } else if (!isKeyWord(tokens[*pos])) {
        return GetAssignment(tokens, pos);
    } else {
        syntaxError(__LINE__);
    }
}

static TNode* GetIf(const std::vector<std::string>& tokens, size_t* pos) {
    size_t op = *pos;
    (*pos)++;
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    TNode* leftNode = GetComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    TNode* rightNode = GetOperation(tokens, pos);

    return NewNode(If, tokens[op], leftNode, rightNode);
}

static TNode* GetWhile(const std::vector<std::string>& tokens, size_t* pos) {
    size_t op = *pos;
    (*pos)++;
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    TNode* leftNode = GetComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    TNode* rightNode = GetOperation(tokens, pos);

    return NewNode(While, tokens[op], leftNode, rightNode);
}

static TNode* GetAssignment(const std::vector<std::string>& tokens, size_t* pos) {
    TNode* leftNode = GetVariable(tokens, pos);
    TNode* rightNode = nullptr;
    if (tokens[*pos] != keyEqual) {
        syntaxError(__LINE__);
    }
    size_t op = *pos;
    (*pos)++;
    if (tokens[*pos] == keyCall) {
        (*pos)++;
        const std::string* name = &tokens[*pos];
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        TNode* argNode = GetVariable(tokens, pos);
        while (tokens[*pos] == keySemicolon) {
            (*pos)++;
            argNode->left = NewNode(Identifier, tokens[*pos], nullptr, nullptr);
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        rightNode = NewNode(Calling, *name, argNode, nullptr); 
    } else if (tokens[*pos] == keyReadInt) {
        size_t op1 = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        rightNode = NewNode(ReadInt, tokens[op1], nullptr, nullptr);
    } else {
        rightNode = GetComparsion(tokens, pos);
    }
    return NewNode(Equal, tokens[op], leftNode, rightNode);
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