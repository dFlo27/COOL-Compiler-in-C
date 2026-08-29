#ifndef parser_h
#define parser_h

#include "lexer.h"

struct AST
{
    struct Token *token;
    struct AST *children;
};

struct AST *parse(struct LexicalToken *);

#endif