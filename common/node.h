#ifndef NODE_H
#define NODE_H

#include <unordered_map>
#include <string_view>
#include <string>

enum NodeType {
    Null = 0,
    Number,
    Identifier,
    Def,
    Calling,
    Semicolon,
    PrintAscii,
    PrintInt,
    ReadInt,
    While,
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
    Call,
    Return,
};

struct tNode {
    NodeType type;
    std::string value;
    tNode* left;
    tNode* right;
};

inline const std::string keyIf = "if";
inline const std::string keyAdd = "+";
inline const std::string keySub = "-";
inline const std::string keyMul = "*";
inline const std::string keyDiv = "/";
inline const std::string keyLess = "<";
inline const std::string keyDef = "def";
inline const std::string keyEnd = "end";
inline const std::string keyEqual = "=";
inline const std::string keyGreater = ">";
inline const std::string keyCall = "call";
inline const std::string keyPrintAscii = "print_ascii";
inline const std::string keyPrintInt = "print_int";
inline const std::string keyReadInt = "read_int";
inline const std::string keyWhile = "while";
inline const std::string keySemicolon = ";";
inline const std::string keyIdentical = "==";
inline const std::string keyReturn = "return";
inline const std::string keyLessOrEqual = "<=";
inline const std::string keyNotIdentical = "!=";
inline const std::string keyGreaterOrEqual = ">=";
inline const std::string keyLeftParenthesis = "(";
inline const std::string keyRightParenthesis = ")";
inline const std::string keyLeftCurlyBracket = "{";
inline const std::string keyRightCurlyBracket = "}";
inline const std::string keyNumber = "number";
inline const std::string keyIdentifier = "identifier";
inline const std::string keyOperation = "operation";
inline const std::string keyCalling = "calling";

const std::unordered_map<NodeType, std::string> kNodeTypeToString {
    {Number, keyNumber},
    {Identifier, keyIdentifier},
    {Def, keyDef},
    {Calling, keyCalling},
    {Semicolon, keySemicolon},
    {PrintAscii, keyPrintAscii},
    {PrintInt, keyPrintInt},
    {ReadInt, keyReadInt},
    {While, keyWhile},
    {If, keyIf},
    {Add, keyAdd},
    {Sub, keySub},
    {Mul, keyMul},
    {Div, keyDiv},
    {Less, keyLess},
    {Equal, keyEqual},
    {Greater, keyGreater},
    {Identical, keyIdentical},
    {LessOrEqual, keyLessOrEqual},
    {NotIdentical, keyNotIdentical},
    {GreaterOrEqual, keyGreaterOrEqual},
    {Call, keyCall},
    {Return, keyReturn},
};

const std::unordered_map<std::string, NodeType> kStringToNodeType {
    {keyNumber, Number},
    {keyIdentifier, Identifier},
    {keyDef, Def},
    {keyCalling, Calling},
    {keySemicolon, Semicolon},
    {keyPrintAscii, PrintAscii},
    {keyPrintInt, PrintInt},
    {keyReadInt, ReadInt},
    {keyWhile, While},
    {keyIf, If},
    {keyAdd, Add},
    {keySub, Sub},
    {keyMul, Mul},
    {keyDiv, Div},
    {keyLess, Less},
    {keyEqual, Equal},
    {keyGreater, Greater},
    {keyIdentical, Identical},
    {keyLessOrEqual, LessOrEqual},
    {keyNotIdentical, NotIdentical},
    {keyGreaterOrEqual, GreaterOrEqual},
    {keyCall, Call},
    {keyReturn, Return},
};

#endif // NODE_H
