
#include "lexer.h"
#include "print_lex.h"

const char *const token_to_string[] = {
    [AT] = "AT",
    [CASE] = "CASE",
    [CLASS] = "CLASS",
    [COMMA] = "COMMA",
    [COLON] = "COLON",
    [DIVISION] = "DIVISION",
    [DOT] = "DOT",
    [EQUALS] = "EQUALS",
    [ELSE] = "ELSE",
    [ESAC] = "ESAC",
    [FALSE] = "FALSE",
    [FI] = "FI",
    [GEQUAL] = "GEQUAL",
    [GTHAN] = "GTHAN",
    [IDENTIFIER] = "IDENTIFIER",
    [IF] = "IF",
    [IN] = "IN",
    [INHERITS] = "INHERITS",
    [INTEGER] = "INTEGER",
    [INTCOMPLEMENT] = "INTCOMPLEMENT",
    [ISVOID] = "ISVOID",
    [LARROW] = "LARROW",
    [LBRACKET] = "LBRACKET",
    [LEQUAL] = "LEQUAL",
    [LET] = "LET",
    [LOOP] = "LOOP",
    [LPAREN] = "LPAREN",
    [LTHAN] = "LTHAN",
    [MINUS] = "MINUS",
    [MULTI] = "MULTI",
    [NEW] = "NEW",
    [NOT] = "NOT",
    [OF] = "OF",
    [PLUS] = "PLUS",
    [POOL] = "POOL",
    [RARROW] = "RARROW",
    [RBRACKET] = "RBRACKET",
    [RPAREN] = "RPAREN",
    [SEMICOLON] = "SEMICOLON",
    [STRING] = "STRING",
    [THEN] = "THEN",
    [TRUE] = "TRUE",
    [TYPE] = "TYPE",
    [WHILE] = "WHILE"};

void print_lex(FILE *out, struct ArrayList *lex_array)
{
    int i = 0;
    struct TokenInfo *lex_tokens = lex_array->array;
    for (int i = 0; i < lex_array->length; i++)
    {
        fprintf(out, "%d\n%s\n", lex_tokens[i].program_line, token_to_string[lex_tokens[i].token_name]);
        if (lex_tokens[i].lexeme != NULL)
            fprintf(out, "%s\n", lex_tokens[i].lexeme);
    }
}