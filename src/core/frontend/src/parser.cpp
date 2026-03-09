#include "parser.h"

#include <iostream>

#include "frontendExceptions.h"

Node* Parser::GetGrammar() {
    if (!tokens.size()) {
        return ast.Create(End, keyEnd);
    }

    Node* left = GetDef();
    if (tokens[pos] != keySemicolon) {
        SyntaxError();
    }
    pos++;
    while (pos < tokens.size()) {
        Node* right = GetDef();
        if (tokens[pos] != keySemicolon) {
            SyntaxError();
        }
        pos++;
        left = ast.Create(Semicolon, keySemicolon, left, right); 
    }

    return left;
}

Node* Parser::GetExpression() {
    Node* left = GetMultiplication();

    while (tokens[pos] == keyAdd || tokens[pos] == keySub) {
        size_t op = pos;
        pos++;
        Node* right = GetMultiplication(); 
        left = ast.Create(kStringToNodeType.at(tokens[op]), tokens[op], left, right);
    }
    return left;
}

Node* Parser::GetComparsion() {
    Node* left = GetExpression();

    if (tokens[pos] == keyLess || tokens[pos] == keyLessOrEqual ||
        tokens[pos] == keyGreater || tokens[pos] == keyGreaterOrEqual ||
        tokens[pos] == keyIdentical || tokens[pos] == keyNotIdentical) 
    {
        size_t op = pos;
        pos++;
        Node* right = GetExpression();
        left = ast.Create(kStringToNodeType.at(tokens[op]), tokens[op], left, right);
    }

    return left;
}

Node* Parser::GetMultiplication() {
    Node* left = GetParentheses();

    while (tokens[pos] == keyMul || tokens[pos] == keyDiv) {
        size_t op = pos;
        pos++;
        Node* right = GetParentheses();
        left = ast.Create(kStringToNodeType.at(tokens[op]), tokens[op], left, right);
    }
    return left;
}

Node* Parser::GetParentheses() {
    if (tokens[pos] == keyLeftParenthesis) {
        pos++;
        Node* node = GetComparsion();
        if (tokens[pos] != keyRightParenthesis) {
            SyntaxError();
        }
        pos++;
        return node;
    } else if (std::isalpha(tokens[pos][0])) {
        return GetVariable();
    } else if (std::isdigit(tokens[pos][0])) {
        return GetNumber();
    } else {
        SyntaxError();
    }
}

Node* Parser::GetNumber() {
    return ast.Create(Number, tokens[pos++]);
}

Node* Parser::GetVariable() {
    return ast.Create(Identifier, tokens[pos++]);
}

Node* Parser::GetDef() {
    if (tokens[pos] == keyDef) {
        pos++;
        size_t nameIndex = pos;
        pos++;

        CHECK_LEFT_PARENTHESIS;
        pos++;

        Node* firstArgNode = tokens[pos] == keyRightParenthesis ? nullptr : ast.Create();
        Node* root = firstArgNode;
        while (tokens[pos] != keyRightParenthesis) {
            root->SetType(Identifier);
            root->SetValue(tokens[pos]);
            pos++;
            if (tokens[pos] == keyComma) {
                pos++;
                root->SetLeft(ast.Create());
                root = root->GetLeft();
            }
        }
        pos++;
        Node* right = GetOperation();
        return ast.Create(Def, tokens[nameIndex], firstArgNode, right);
    }
    return GetOperation();
}

Node* Parser::GetOperation() {
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
        Node* left = GetComparsion();
        CHECK_RIGHT_PARENTHESIS;
        pos++;
        return ast.Create(PrintAscii, keyPrintAscii, left, nullptr); 
    } else if (tokens[pos] == keyPrintInt) {
        size_t op = pos;
        pos++;
        CHECK_LEFT_PARENTHESIS;
        pos++;
        Node* left = GetComparsion();
        CHECK_RIGHT_PARENTHESIS;
        pos++;
        return ast.Create(PrintInt, keyPrintInt, left, nullptr); 
    } else if (tokens[pos] == keyReturn) {
        size_t op = pos;
        pos++;
        Node* left = GetComparsion();
        return ast.Create(Return, keyReturn, left, nullptr);
    } else if (tokens[pos] == keyLeftCurlyBracket) {
        pos++;
        Node* left = GetOperation();
        if (tokens[pos] != keySemicolon) {
            SyntaxError();
        }
        pos++;
        while (tokens[pos] != keyRightCurlyBracket) {
            Node* righNode = GetOperation();
            if (tokens[pos] != keySemicolon) {
                SyntaxError();
            }
            pos++;
            left = ast.Create(Semicolon, keySemicolon, left, righNode);
        }

        if (tokens[pos] != keyRightCurlyBracket) {
            SyntaxError();
        }
        pos++;

        return left;
    } else if (!kStringToNodeType.contains(tokens[pos])) { 
        return GetAssignment();
    } else {
        SyntaxError();
    }
}

Node* Parser::GetIf() {
    size_t op = pos;
    pos++;
    CHECK_LEFT_PARENTHESIS;
    pos++;
    Node* left = GetComparsion();
    CHECK_RIGHT_PARENTHESIS;
    pos++;

    Node* right = GetOperation();

    return ast.Create(If, keyIf, left, right);
}

Node* Parser::GetWhile() {
    size_t op = pos;
    pos++;
    CHECK_LEFT_PARENTHESIS;
    pos++;
    Node* left = GetComparsion();
    CHECK_RIGHT_PARENTHESIS;
    pos++;

    Node* right = GetOperation();

    return ast.Create(While, keyWhile, left, right);
}

Node* Parser::GetAssignment() {
    Node* lefNode = GetVariable();
    Node* rightNode = nullptr;

    if (tokens[pos] != keyEqual) {
        SyntaxError();
    }

    pos++;
    if (tokens[pos] == keyCall) {
        rightNode = GetCalling();
    } else if (tokens[pos] == keyReadInt) {
        pos++;
        CHECK_LEFT_PARENTHESIS;
        pos++;
        CHECK_RIGHT_PARENTHESIS;
        pos++;
        rightNode = ast.Create(ReadInt, keyReadInt);
    } else {
        rightNode = GetComparsion();
    }
    return ast.Create(Equal, keyEqual, lefNode, rightNode);
}

Node* Parser::GetCalling() {
    pos++;
    size_t nameIndex = pos;
    pos++;
    CHECK_LEFT_PARENTHESIS;
    pos++;
    Node* argNode = tokens[pos] == keyRightParenthesis ? nullptr : ast.Create();
    Node* node = argNode;
    while (tokens[pos] != keyRightParenthesis) {
        node->SetType(Identifier);
        node->SetValue(tokens[pos]);
        pos++;
        if (tokens[pos] == keyComma) {
            pos++;
            node->SetLeft(ast.Create());
            node = node->GetLeft();
        }
    }
    CHECK_RIGHT_PARENTHESIS;
    pos++;
    return ast.Create(Call, tokens[nameIndex], argNode, nullptr);
}

void Parser::SyntaxError() {
    throw FrontendExcept::ParserException("Syntax error: \"" + tokens[pos] + "\"");
}
