#ifndef token_h
#define token_h

enum TOKEN
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

struct TokenInfo
{
    char *lexeme;
    enum TOKEN token_name;
    int program_line;
};

#endif