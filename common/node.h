#ifndef NODE_H
#define NODE_H

#include <unordered_map>
#include <string>
#include <memory>

enum NodeType {
    Null = 0,
    End,
    Semicolon,
    Number,
    Identifier,
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
    Def,
    Call,
    Return,
};

class Node {
private:
    NodeType type;
    std::string value;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;

public:
    Node(NodeType t, const std::string& v, std::unique_ptr<Node> l, std::unique_ptr<Node> r)
        : type(t), value(v), left(std::move(l)), right(std::move(r)) {}

    Node(NodeType t, const std::string& v)
        : type(t), value(v), left(nullptr), right(nullptr) {}

    Node() : left(nullptr), right(nullptr) {}

    void SetType(NodeType t) {
        type = t;
    }

    void SetValue(const std::string& v) {
        value = v;
    }

    void SetLeft(std::unique_ptr<Node> l) {
        left = std::move(l);
    }

    void SetRight(std::unique_ptr<Node> r) {
        right = std::move(r);
    }

    NodeType GetType() const {
        return type;
    }

    std::string GetValue() const {
        return value;
    }

    Node* GetLeft() const {
        return left.get();
    }

    Node* GetRight() const {
        return right.get();
    }
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
inline const std::string keyComma = ",";

const std::unordered_map<NodeType, std::string> kNodeTypeToString {
    {End, keyEnd},
    {Semicolon, keySemicolon},
    {Number, keyNumber},
    {Identifier, keyIdentifier},
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
    {Def, keyDef},  
    {Call, keyCall},
    {Return, keyReturn},
};

const std::unordered_map<std::string, NodeType> kStringToNodeType {
    {keyNumber, Number},
    {keyIdentifier, Identifier},
    {keyDef, Def},
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
    {keyEnd, End},
};

#endif // NODE_H
