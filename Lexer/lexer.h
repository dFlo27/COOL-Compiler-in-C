#ifndef lexer_h
#define lexer_h

enum TOKEN_TYPE
{
    AT,
    CASE,
    CLASS,
    COMMA,
    COLON,
    DIVISION,
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
    MULTI,
    NEW,
    NOT,
    OF,
    PLUS,
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

struct LexicalToken
{
    char *token_literal;
    enum TOKEN_TYPE type;
    int program_line;
};

struct LexicalToken **lex(FILE *source_file);

#endif