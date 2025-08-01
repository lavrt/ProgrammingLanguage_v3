#ifndef PARSER_H
#define PARSER_H

#include "node.h"

#include <vector>

#define CHECK_LEFT_PARENTHESIS \
    do { if (strcmp(tokens[*pos], "(")) syntaxError(__LINE__); } while(0);
#define CHECK_RIGHT_PARENTHESIS \
    do { if (strcmp(tokens[*pos], ")")) syntaxError(__LINE__); } while(0);

tNode* runParser(const std::vector<char*>& tokens);

#endif // PARSER_H
