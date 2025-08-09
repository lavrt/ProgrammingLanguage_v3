#ifndef PARSER_H
#define PARSER_H

#include "node.h"

#include <vector>
#include <string>

#define CHECK_LEFT_PARENTHESIS \
    do { if (tokens[*pos] != keyLeftParenthesis) syntaxError(__LINE__); } while(0);
#define CHECK_RIGHT_PARENTHESIS \
    do { if (tokens[*pos] != keyRightParenthesis) syntaxError(__LINE__); } while(0);

tNode* runParser(const std::vector<std::string>& tokens);

#endif // PARSER_H
