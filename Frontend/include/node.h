#ifndef NODE_H
#define NODE_H

enum NodeType {
    Number     = 1,
    Operation  = 2,
    Identifier = 3,
    Function   = 4,
    Calling    = 5,
    Binary     = 6,
};

struct tNode {
    NodeType type;
    const char* value;
    tNode* left;
    tNode* right;
};

enum Operations {
    NoOperation = 0,
    Semicolon,
    PrintAscii,
    PrintInt,
    While,
    Sqrt,
    Sin,
    Cos,
    If,
    Add,
    Sub,
    Mul,
    Div,
    Less,
    Equal,
    Greater,
    Identical,
    LessOrEqual,
    NotIdentical,
    GreaterOrEqual,
    Def,
    Call,
    Return,
};

const char* const keyIf = "if";
const char* const keyAdd = "+";
const char* const keySub = "-";
const char* const keyMul = "*";
const char* const keyDiv = "/";
const char* const keyLess = "<";
const char* const keyDef = "def";
const char* const keySin = "sin";
const char* const keyCos = "cos";
const char* const keyEnd = "end";
const char* const keyEqual = "=";
const char* const keyGreater = ">";
const char* const keyCall = "call";
const char* const keySqrt = "sqrt";
const char* const keyPrintAscii = "print_ascii";
const char* const keyPrintInt = "print_int";
const char* const keyWhile = "while";
const char* const keySemicolon = ";";
const char* const keyIdentical = "==";
const char* const keyReturn = "return";
const char* const keyLessOrEqual = "<=";
const char* const keyNotIdentical = "!=";
const char* const keyGreaterOrEqual = ">=";
const char* const keyLeftParenthesis = "(";
const char* const keyRightParenthesis = ")";
const char* const keyLeftCurlyBracket = "{";
const char* const keyRightCurlyBracket = "}";

#endif // NODE_H
