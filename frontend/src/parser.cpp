#include "parser.h"

#include <iostream>

#include "frontendExceptions.h"

std::unique_ptr<Node> Parser::GetGrammar() {
    if (!tokens.size()) {
        return std::make_unique<Node>(End, keyEnd);
    }

    std::unique_ptr<Node> left = GetDef();
    if (tokens[pos] != keySemicolon) {
        SyntaxError();
    }
    pos++;
    while (pos < tokens.size()) {
        std::unique_ptr<Node> right = GetDef();
        if (tokens[pos] != keySemicolon) {
            SyntaxError();
        }
        pos++;
        left = std::make_unique<Node>(Semicolon, keySemicolon, std::move(left), std::move(right)); 
    }

    return left;
}

std::unique_ptr<Node> Parser::GetExpression() {
    std::unique_ptr<Node> left = GetMultiplication();

    while (tokens[pos] == keyAdd || tokens[pos] == keySub) {
        size_t op = pos;
        pos++;
        std::unique_ptr<Node> right = GetMultiplication(); 
        left = std::make_unique<Node>(kStringToNodeType.at(tokens[op]), tokens[op], std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::GetComparsion() {
    std::unique_ptr<Node> left = GetExpression();

    if (tokens[pos] == keyLess || tokens[pos] == keyLessOrEqual ||
        tokens[pos] == keyGreater || tokens[pos] == keyGreaterOrEqual ||
        tokens[pos] == keyIdentical || tokens[pos] == keyNotIdentical) 
    {
        size_t op = pos;
        pos++;
        std::unique_ptr<Node> right = GetExpression();
        left = std::make_unique<Node>(kStringToNodeType.at(tokens[op]), tokens[op], std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Node> Parser::GetMultiplication() {
    std::unique_ptr<Node> left = GetParentheses();

    while (tokens[pos] == keyMul || tokens[pos] == keyDiv) {
        size_t op = pos;
        pos++;
        std::unique_ptr<Node> right = GetParentheses();
        left = std::make_unique<Node>(kStringToNodeType.at(tokens[op]), tokens[op], std::move(left), std::move(right));
    }
    return left;
}

std::unique_ptr<Node> Parser::GetParentheses() {
    if (tokens[pos] == keyLeftParenthesis) {
        pos++;
        std::unique_ptr<Node> node = GetComparsion();
        if (tokens[pos] != keyRightParenthesis) {
            SyntaxError();
        }
        pos++;
        return node;
    } else if (!std::isdigit(tokens[pos][0])) {
        return GetVariable();
    } else if (std::isdigit(tokens[pos][0])) {
        return GetNumber();
    } else {
        SyntaxError();
    }
}

std::unique_ptr<Node> Parser::GetNumber() {
    return std::make_unique<Node>(Number, tokens[pos++]);
}

std::unique_ptr<Node> Parser::GetVariable() {
    return std::make_unique<Node>(Identifier, tokens[pos++]);
}

std::unique_ptr<Node> Parser::GetDef() {
    if (tokens[pos] == keyDef) {
        pos++;
        size_t nameIndex = pos;
        pos++;

        CHECK_LEFT_PARENTHESIS;
        pos++;

        std::unique_ptr<Node> firstArgNode = tokens[pos] == keyRightParenthesis ? nullptr : std::make_unique<Node>();
        Node* root = firstArgNode.get();
        while (tokens[pos] != keyRightParenthesis) {
            root->SetType(Identifier);
            root->SetValue(tokens[pos]);
            pos++;
            if (tokens[pos] == keyComma) {
                pos++;
                root->SetLeft(std::make_unique<Node>());
                root = root->GetLeft();
            }
        }
        pos++;
        std::unique_ptr<Node> right = GetOperation();
        return std::make_unique<Node>(Def, tokens[nameIndex], std::move(firstArgNode), std::move(right));
    }
    return GetOperation();
}

std::unique_ptr<Node> Parser::GetOperation() {
    if (tokens[pos] == keyCall) {
        return GetCalling();
    } else if (tokens[pos] == keyIf) {
        return GetIf();
    } else if (tokens[pos] == keyWhile) {
        return GetWhile();
    } else if (tokens[pos] == keyPrintAscii) {
        size_t op = pos;
        pos++;
        CHECK_LEFT_PARENTHESIS;
        pos++;
        std::unique_ptr<Node> left = GetComparsion();
        CHECK_RIGHT_PARENTHESIS;
        pos++;
        return std::make_unique<Node>(PrintAscii, keyPrintAscii, std::move(left), nullptr); 
    } else if (tokens[pos] == keyPrintInt) {
        size_t op = pos;
        pos++;
        CHECK_LEFT_PARENTHESIS;
        pos++;
        std::unique_ptr<Node> left = GetComparsion();
        CHECK_RIGHT_PARENTHESIS;
        pos++;
        return std::make_unique<Node>(PrintInt, keyPrintInt, std::move(left), nullptr); 
    } else if (tokens[pos] == keyReturn) {
        size_t op = pos;
        pos++;
        std::unique_ptr<Node> left = GetComparsion();
        return std::make_unique<Node>(Return, keyReturn, std::move(left), nullptr);
    } else if (tokens[pos] == keyLeftCurlyBracket) {
        pos++;
        std::unique_ptr<Node> left = GetOperation();
        if (tokens[pos] != keySemicolon) {
            SyntaxError();
        }
        pos++;
        while (tokens[pos] != keyRightCurlyBracket) {
            std::unique_ptr<Node> righNode = GetOperation();
            if (tokens[pos] != keySemicolon) {
                SyntaxError();
            }
            pos++;
            left = std::make_unique<Node>(Semicolon, keySemicolon, std::move(left), std::move(righNode));
        }

        if (tokens[pos] != keyRightCurlyBracket) {
            SyntaxError();
        }
        pos++;

        return left;
    } else if (!isKeyWord(tokens[pos])) { 
        return GetAssignment();
    } else {
        SyntaxError();
    }
}

std::unique_ptr<Node> Parser::GetIf() {
    size_t op = pos;
    pos++;
    CHECK_LEFT_PARENTHESIS;
    pos++;
    std::unique_ptr<Node> left = GetComparsion();
    CHECK_RIGHT_PARENTHESIS;
    pos++;

    std::unique_ptr<Node> right = GetOperation();

    return std::make_unique<Node>(If, keyIf, std::move(left), std::move(right));
}

std::unique_ptr<Node> Parser::GetWhile() {
    size_t op = pos;
    pos++;
    CHECK_LEFT_PARENTHESIS;
    pos++;
    std::unique_ptr<Node> left = GetComparsion();
    CHECK_RIGHT_PARENTHESIS;
    pos++;

    std::unique_ptr<Node> right = GetOperation();

    return std::make_unique<Node>(While, keyWhile, std::move(left), std::move(right));
}

std::unique_ptr<Node> Parser::GetAssignment() {
    std::unique_ptr<Node> lefNode = GetVariable();
    std::unique_ptr<Node> righNode = nullptr;

    if (tokens[pos] != keyEqual) {
        SyntaxError();
    }

    pos++;
    if (tokens[pos] == keyCall) {
        righNode = GetCalling();
    } else if (tokens[pos] == keyReadInt) {
        pos++;
        CHECK_LEFT_PARENTHESIS;
        pos++;
        CHECK_RIGHT_PARENTHESIS;
        pos++;
        righNode = std::make_unique<Node>(ReadInt, keyReadInt);
    } else {
        righNode = GetComparsion();
    }
    return std::make_unique<Node>(Equal, keyEqual, std::move(lefNode), std::move(righNode));
}

std::unique_ptr<Node> Parser::GetCalling() {
    pos++;
    size_t nameIndex = pos;
    pos++;
    CHECK_LEFT_PARENTHESIS;
    pos++;
    std::unique_ptr<Node> argNode = tokens[pos] == keyRightParenthesis ? nullptr : std::make_unique<Node>();
    Node* node = argNode.get();
    while (tokens[pos] != keyRightParenthesis) {
        node->SetType(Identifier);
        node->SetValue(tokens[pos]);
        pos++;
        if (tokens[pos] == keyComma) {
            pos++;
            node->SetLeft(std::make_unique<Node>());
            node = node->GetLeft();
        }
    }
    CHECK_RIGHT_PARENTHESIS;
    pos++;
    return std::make_unique<Node>(Call, tokens[nameIndex], std::move(argNode), nullptr);
}

void Parser::SyntaxError() {
    throw FrontendExcept::ParserException("Syntax error: \"" + tokens[pos] + "\"");
}

bool Parser::isKeyWord(const std::string& word) {
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