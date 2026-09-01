#ifndef lexer_h
#define lexer_h

#include "token.h"
#include <stdio.h>

struct ArrayList *lexer(FILE *source_file);

#endif