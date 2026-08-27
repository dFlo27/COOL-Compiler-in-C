#ifndef lexer_h
#define lexer_h

enum TOKEN_TYPE
{
    AT,
    CASE,
    CLASS,
    COLON,
    DOT,
    EQUALS,
    ELSE,
    ESAC,
    FALSE,
    FI,
    GEQUAL,
    GTHAN,
    IDENTIFIER,
    IF,
    IN,
    INHERITS,
    INTEGER,
    INTCOMPLEMENT,
    ISVOID,
    LARROW,
    LBRACKET,
    LEQUAL,
    LET,
    LOOP,
    LPAREN,
    LTHAN,
    MINUS,
    NEW,
    NOT,
    OF,
    POOL,
    RARROW,
    RBRACKET,
    RPAREN,
    SEMICOLON,
    STRING,
    THEN,
    TRUE,
    TYPE,
    WHILE
};

void lex(FILE *source_file);

#endif