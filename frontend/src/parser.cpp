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

    TNode* lefTNode = GetDef(tokens, &pos);
    if (tokens[pos++] != keySemicolon) {
        syntaxError(__LINE__);
    }
    while (tokens[pos] != keyEnd) {
        TNode* righTNode = GetDef(tokens, &pos);
        if (tokens[pos] != keySemicolon) {
            syntaxError(__LINE__);
        }
        lefTNode = NewNode(Semicolon, tokens[pos++], lefTNode, righTNode);
    }

    return lefTNode;
}

static TNode* GetExpression(const std::vector<std::string>& tokens, size_t* pos) {
    TNode* lefTNode = GetMultiplication(tokens, pos);

    while (tokens[*pos] == keyAdd || tokens[*pos] == keySub) {
        size_t op = *pos;
        (*pos)++;
        TNode* righTNode = GetMultiplication(tokens, pos); 
        lefTNode = NewNode(kStringToNodeType.at(tokens[op]), tokens[op], lefTNode, righTNode);
    }
    return lefTNode;
}

static TNode* GetComparsion(const std::vector<std::string>& tokens, size_t* pos) {
    TNode* lefTNode = GetExpression(tokens, pos);

    if (tokens[*pos] == keyLess ||
        tokens[*pos] == keyGreater || 
        tokens[*pos] == keyIdentical ||
        tokens[*pos] == keyLessOrEqual ||
        tokens[*pos] == keyGreaterOrEqual ||
        tokens[*pos] == keyNotIdentical) 
    {
        size_t op = *pos;
        (*pos)++;
        TNode* righTNode = GetExpression(tokens, pos);
        lefTNode = NewNode(kStringToNodeType.at(tokens[op]), tokens[op], lefTNode, righTNode);
    }

    return lefTNode;
}

static TNode* GetMultiplication(const std::vector<std::string>& tokens, size_t* pos) {
    TNode* lefTNode = GetParentheses(tokens, pos);

    while (tokens[*pos] == keyMul || tokens[*pos] == keyDiv) {
        size_t op = *pos;
        (*pos)++;
        TNode* righTNode = GetParentheses(tokens, pos);
        lefTNode = NewNode(kStringToNodeType.at(tokens[op]), tokens[op], lefTNode, righTNode);
    }
    return lefTNode;
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
        TNode* lefTNode = GetVariable(tokens, pos);
        TNode* node = lefTNode;
        while (tokens[*pos] == keySemicolon) {
            (*pos)++;
            lefTNode->left = NewNode(Identifier, tokens[*pos], nullptr, nullptr);
            lefTNode = lefTNode->left;
            (*pos)++;
        }
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        TNode* righTNode = GetOperation(tokens, pos);
        return NewNode(Def, *name, node, righTNode);
    }
    TNode* lefTNode = GetOperation(tokens, pos);
    return lefTNode;
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
        TNode* lefTNode = GetComparsion(tokens, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        return NewNode(PrintAscii, tokens[op], lefTNode, nullptr); 
    } else if (tokens[*pos] == keyPrintInt) {
        size_t op = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        TNode* lefTNode = GetComparsion(tokens, pos);
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        return NewNode(PrintInt, tokens[op], lefTNode, nullptr);
    } else if (tokens[*pos] == keyReturn) {
        size_t op = *pos;
        (*pos)++;
        TNode* lefTNode = GetComparsion(tokens, pos);
        return NewNode(Return, tokens[op], lefTNode, nullptr);
    } else if (tokens[*pos] == keyLeftCurlyBracket) {
        (*pos)++;
        TNode* lefTNode = GetOperation(tokens, pos);
        if (tokens[(*pos)++] != keySemicolon) {
            syntaxError(__LINE__);
        }
        while (tokens[*pos] != keyRightCurlyBracket) {
            TNode* righTNode = GetOperation(tokens, pos);
            if (tokens[(*pos)] != keySemicolon) {
                syntaxError(__LINE__);
            }
            lefTNode = NewNode(Semicolon, tokens[(*pos)++], lefTNode, righTNode);
        }

        if (tokens[(*pos)++] != keyRightCurlyBracket) {
            syntaxError(__LINE__);
        }

        return lefTNode;
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
    TNode* lefTNode = GetComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    TNode* righTNode = GetOperation(tokens, pos);

    return NewNode(If, tokens[op], lefTNode, righTNode);
}

static TNode* GetWhile(const std::vector<std::string>& tokens, size_t* pos) {
    size_t op = *pos;
    (*pos)++;
    CHECK_LEFT_PARENTHESIS;
    (*pos)++;
    TNode* lefTNode = GetComparsion(tokens, pos);
    CHECK_RIGHT_PARENTHESIS;
    (*pos)++;

    TNode* righTNode = GetOperation(tokens, pos);

    return NewNode(While, tokens[op], lefTNode, righTNode);
}

static TNode* GetAssignment(const std::vector<std::string>& tokens, size_t* pos) {
    TNode* lefTNode = GetVariable(tokens, pos);
    TNode* righTNode = nullptr;
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
        righTNode = NewNode(Calling, *name, argNode, nullptr); 
    } else if (tokens[*pos] == keyReadInt) {
        size_t op1 = *pos;
        (*pos)++;
        CHECK_LEFT_PARENTHESIS;
        (*pos)++;
        CHECK_RIGHT_PARENTHESIS;
        (*pos)++;
        righTNode = NewNode(ReadInt, tokens[op1], nullptr, nullptr);
    } else {
        righTNode = GetComparsion(tokens, pos);
    }
    return NewNode(Equal, tokens[op], lefTNode, righTNode);
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