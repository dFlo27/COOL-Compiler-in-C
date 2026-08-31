#ifndef lexer_h
#define lexer_h

#include <stdio.h>
#include "token.h"

struct Lexeme
{
    char *token_literal;
    enum TOKEN_NAME token_name;
    int program_line;
};

struct ArrayList *lexer(FILE *source_file);

#endif