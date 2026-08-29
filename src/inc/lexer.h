#ifndef lexer_h
#define lexer_h

#include <stdio.h>
#include "token.h"

struct LexicalToken
{
    char *token_literal;
    enum TOKEN_TYPE type;
    int program_line;
};

struct LexicalToken **lex(FILE *source_file);

#endif