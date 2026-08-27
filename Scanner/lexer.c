#include <stdio.h>
#include <stdlib.h>
#include "lexer.h";

#define INT_MAX 2147483647
#define INT_MIN (-2147483648)
#define INITIAL_STRING_LENGTH 8
#define INITIAL_LIST_LENGTH 8

struct String
{
    char *s;
    int length;
    int size;
};

struct LexicalToken
{
    char *token_literal;
    enum TOKEN_TYPE type;
    int program_line;
};

void initialize_token(struct LexicalToken *t, int program_line, enum TOKEN_TYPE type)
{
    t->program_line = program_line;
    t->token_literal = NULL;
    t->type = type;
}

void initialize_string(struct String *str)
{
    str->length = 0;
    str->s = malloc(INITIAL_STRING_LENGTH * sizeof(char));
    str->size = INITIAL_STRING_LENGTH;
    str->s[0] = '\0';
}

void append_char(struct String *str, char c)
{
    if (str->length == str->size - 1)
    {
        str->size *= 2;
        str->s = realloc(str->s, str->size * sizeof(char));
        if (str->s == NULL)
        {
            printf("Ran out of memory allocating a new character\n");
            /*Change Number*/ exit(1);
        }
    }
    str->s[str->length++] = c;
    str->s[str->length] = '\0';
}

// Checks if two strings are equal. Assumes both strings are same length because I can assume so.
int str_equals(const char *str1, const char *str2)
{
    int index;
    char c;
    for (index = 0; str1[index] != '\0'; index++)
    {
        if (str1[index] != str2[index])
            return 0;
    }
    return 1;
};

struct LexicalToken *lex(FILE *source)
{
    int c, list_length, list_size, program_line;
    struct LexicalToken **token_list;
    struct LexicalToken *token;
    struct String new_string;

    token_list = malloc(INITIAL_LIST_LENGTH * sizeof(struct LexicalToken *));
    list_size = INITIAL_LIST_LENGTH;
    list_size = 0;
    program_line = 1;

    while ((c = fgetc(source)) != EOF)
    {
        switch (c)
        {
        // Inline Whitespace
        case '\t':
        case '\v':
        case '\f':
        case '\r':
        case ' ':
            break; /* Do Nothing */

        // New Line Whitespace
        case '\n':
            program_line++;
            break;

        // String Constant
        case '\"':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, STRING);
            append_token(token_list, token);
            initialize_string(&new_string);

            while ((c = fgetc(source)) != "\"")
            {
                if (c == 0 || c == EOF)
                {
                    printf("ERROR: %d: Lexer: missing \" character");
                    /* Change Number */ exit(1);
                }
                append_char(&new_string, c);
            }
            token->token_literal = new_string.s;
            break;

        // Left Parenthesis Symbol
        case '(':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, LPAREN);
            append_token(token_list, token);
            // Comment Case 
            if ((c = fgetc(source)) == '*')
            {
                while ((c = fgetc(source)) != EOF )
                {
                    if (c == '*' && (c = fgetc(source)) == ')')
                        break;
                    if (c == '\n')
                        program_line++;
                    
                }
                if (c == EOF)
                {
                    printf("ERROR %d: Lexer: no terminating ", program_line);
                    /* Change Number */ exit(1);
                }
            }
            else
                ungetc(c, source);
            break;

        // Right Parenthesis Symbol
        case ')':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, RPAREN);
            append_token(token_list, token);
            break;

        case '*':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, RPAREN);
            append_token(token_list, token);

            break;

        case '-':
            switch (c = fgetc(source))
            {
            // Minus Operator
            case ' ':
                initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, MINUS);
                append_token(token_list, token);
                break;
            // Comment, Ignore everything until EOF or newline
            case '-':
                while ((c = fgetc(source)) != '\n' || c != EOF)
                    ungetc(c, source);
                break;
            // Negative Integer Constant
            case '0' ... '9':
                initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, INTEGER);
                append_token(token_list, token);
                initialize_string(&new_string);
                append_char(&new_string, '-');
                do
                {
                    if (new_string.length > 9 && strtol(new_string.s, NULL, 10) <= INT_MIN)
                    {
                        printf("ERROR: %d: Lexer: invalid integer %s\n", program_line, new_string.s);
                        /* Change Number */ exit(1);
                    }
                    append_char(&new_string, c);
                } while ((c = fgetc(source) >= '0' && c <= '9'));
                ungetc(c, source);
                token->token_literal = new_string.s;
                break;
            default:
                initialize_string(&new_string);
                append_char(&new_string, '-');
                append_char(&new_string, c);
                printf("ERROR: %d: Lexer: Invalid Syntax: %s\n", program_line, new_string.s);
                /* Change Number */ exit(1);
            }
            break;

        // Dot Syntax
        case '.':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, DOT);
            append_token(token_list, token);
            break;

        // Positive Integer Constant
        case '0' ... '9':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, INTEGER);
            append_token(token_list, token);
            initialize_string(&new_string);

            do
            {
                if (new_string.length > 8 && strtol(new_string.s, NULL, 10) >= INT_MAX)
                {
                    printf("ERROR: %d: Lexer: invalid integer %s\n", program_line, new_string.s);
                    /* Change Number */ exit(1);
                }
                append_char(&new_string, c);
            } while ((c = fgetc(source)) >= '0' && c <= '9');

            ungetc(c, source);
            token->token_literal = new_string.s;
            break;

        case ':':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, COLON);
            append_token(token_list, token);
            break;

        case ';':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, SEMICOLON);
            append_token(token_list, token);
            break;

        case '<':
            token = malloc(sizeof(struct LexicalToken));
            append(token_list, token);
            token->program_line = program_line;
            token->token_literal = NULL;
            switch (c = fgetc(source))
            {
            // Left Arrow Symbol ( <- )
            case '-':
                token->type = LARROW;
                break;
            // Less Than or Equal to Symbol ( <= )
            case '=':
                token->type = LEQUAL;
                break;
            // Less Than Symbol ( < )
            default:
                fputc(c, source);
                token->type = LTHAN;
            }
            break;

        case '=':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, EQUALS);
            append_token(token_list, token);
            // Right Arrow Symbol ( => )
            if ((c = fgetc(source)) == '>')
                token->type = RARROW;
            // Equals Symbol
            else
                ungetc(c, source);
            break;

        case '>':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, GTHAN);
            append_token(token_list, token);
            // Greater Than or Equal Sign
            if ((c = fgetc(source)) == '=')
                token->type = GEQUAL;
            // Greater Than Sign
            else
                ungetc(c, source);
            break;

        // AT Symbol
        case '@':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, AT);
            append_token(token_list, token);
            break;

        // Type Symbols
        case 'A' ... 'Z':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, TYPE);
            append_token(token_list, token);
            initialize_string(&new_string);
            do
            {
                append_char(&new_string, c);
            } while (((c = fgetc(source)) >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
            ungetc(c, source);
            token->token_literal = new_string.s;
            break;

        // Identifiers
        case 'a' ... 'z':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, IDENTIFIER);
            append_token(token_list, token);
            initialize_string(&new_string);
            do
            {
                append_char(&new_string, c);
            } while (((c = fgetc(source)) >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
            ungetc(c, source);
            // Check if identifier is actually a keyword
            switch (new_string.length)
            {
            case 2:
                if (new_string.s[0] == 'i')
                {
                    if (new_string.s[1] == 'f')
                        token->type = IF;
                    else if (new_string.s[1] == 'n')
                        token->type = IN;
                }
                else if (str_equals(new_string.s, "of"))
                    token->type = OF;
                else if (str_equals(new_string.s, "fi"))
                    token->type = FI;
                break;
            case 3:
                if (str_equals(new_string.s, "let"))
                    token->type = LET;
                else if (new_string.s[0] == 'n')
                {
                    if (str_equals(new_string.s[1], "ew"))
                        token->type = NEW;
                    else if (str_equals(new_string.s[1], "ot"))
                        token->type = NOT;
                }
                break;
            case 4:
                if (new_string.s[0] == 'c')
                {
                    if (str_equals(new_string.s[1], "ase"))
                        token->type = CASE;
                    else if (str_equals(new_string.s[1], "lass"))
                        token->type = CLASS;
                }
                else if (new_string.s[0] == 'e')
                {
                    if (str_equals(new_string.s[1], "lse"))
                        token->type = ELSE;
                    else if (str_equals(new_string.s[1], "sac"))
                        token->type = ESAC;
                }
                else if (str_equals(new_string.s, "loop"))
                    token->type = LOOP;
                else if (new_string.s[0] == 't')
                {
                    if ((new_string.s[1] == 'r' || new_string.s[1] == 'R') && (new_string.s[2] == 'u' || new_string.s[2] == 'U') && (new_string.s[3] == 'e' || new_string.s[3] == 'E'))
                        token->type = TRUE;
                    else if (str_equals(new_string.s[1], 'hen'))
                        token->type = THEN;
                }
                else if (str_equals(new_string.s, "pool"))
                    token->type = POOL;
                break;
            case 5:
                if (new_string.s[0] == 'f' && (new_string.s[1] == 'a' || new_string.s[1] == 'A') && (new_string.s[2] == 'l' || new_string.s[2] == 'L') && (new_string.s[3] == 's' || new_string.s[3] == 'S') && (new_string.s[4] == 'e' || new_string.s[4] == 'E'))
                    token->type = FALSE;
                else if (str_equals(new_string.s, "while"))
                    token->type = WHILE;
                break;
            case 6:
                if (str_equals(new_string.s, "isvoid"))
                    token->type = ISVOID;
                break;
            case 8:
                if (str_equals(new_string.s, "inherits"))
                    token->type = INHERITS;
            }

            if (token->type == IDENTIFIER)
                token->token_literal = new_string.s;
            else
                free(new_string.s);
            break;

        // Left Bracket Symbol
        case '{':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, LBRACKET);
            append_token(token_list, token);
            break;

        // Right Bracket Symbol
        case '}':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, RBRACKET);
            append_token(token_list, token);
            break;

        // Integer Complement Symbol
        case '~':
            initialize_token((token = malloc(sizeof(struct LexicalToken))), program_line, INTCOMPLEMENT);
            append_token(token_list, token);
            break;

        // Unidentified Character
        default:
            printf("ERROR: %d: Lexer: invalid character %c", program_line, c);
            /* Change Numebr */ exit(1);
        }
    }
    return token_list;
};