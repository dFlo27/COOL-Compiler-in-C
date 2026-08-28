#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"

#define INITIAL_LIST_LENGTH 8

struct ArrayList
{
    void *array;
    size_t length;
    size_t size;
    size_t element_size;
};

struct LexicalToken *create_token(int program_line, enum TOKEN_TYPE type)
{
    struct LexicalToken *t = malloc(sizeof(struct LexicalToken));
    if (t == NULL)
    {
        printf("Malloc failed to allocate space\n");
        /* Change Number */ exit(1);
    }
    t->program_line = program_line;
    t->token_literal = NULL;
    t->type = type;
    return t;
}

void initialize_list(struct ArrayList *list, size_t element_size)
{
    list->array = malloc(INITIAL_LIST_LENGTH * element_size);
    if (list->array == NULL)
    {
        printf("Malloc failed to allocate space\n");
        /* Change Number */ exit(1);
    }
    list->length = 0;
    list->size = INITIAL_LIST_LENGTH;
    list->element_size = element_size;
}

void realloc_list(struct ArrayList *list, size_t new_size)
{
    void *new_loc;
    size_t list_byte_length = new_size * list->element_size;
    list->size = new_size;

    new_loc = realloc(list->array, list_byte_length);
    if (new_loc == NULL)
    {
        new_loc = malloc(list_byte_length);
        if (new_loc == NULL)
        {
            printf("Malloc failed to allocate space\n");
            /* Change Number */ exit(1);
        }
        memcpy(new_loc, list->array, list_byte_length);
        free(list->array);
    }
    list->array = new_loc;
}

void append_list(struct ArrayList *list, void *element)
{
    if (list->length == list->size)
        realloc_list(list, list->size * 2);
    memcpy(list->array + list->length * list->element_size, &element, list->element_size);
    list->length++;
}

int str_equal(const char *str1, const char *str2) {
    int i;
    for (i = 0; str1[i] == str2[i] && str1[i] != '\0'; i++);
    return str1[i] == str2[i];
}

struct LexicalToken **lex(FILE *source)
{
    void *temp;
    int c, program_line;
    struct ArrayList token_list;
    struct LexicalToken *token;
    struct ArrayList new_string;

    // Program File starts at line 1
    program_line = 1;
    initialize_list(&token_list, sizeof(struct LexicalToken *));

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
            token = create_token(program_line, STRING);
            append_list(&token_list, token);
            initialize_list(&new_string, sizeof(char));

            while ((c = fgetc(source)) != '\"')
            {
                switch (c)
                {
                case '\0':
                case '\n':
                case EOF:
                    /* Change Number */ exit(1);
                    break;
                case '\\':
                    append_list(&new_string, (void *)(long long){c});
                    c = fgetc(source);
                    if (c == '\"')
                        append_list(&new_string, (void *)(long long){c});
                    else
                        ungetc(c, source);
                    break;
                default:
                    append_list(&new_string, (void *)(long long){c});
                }
            }
            append_list(&new_string, '\0');
            realloc_list(&new_string, new_string.length);
            token->token_literal = (char *)new_string.array;
            break;

        // Left Parenthesis Symbol
        case '(':
            // Comment Case
            if ((c = fgetc(source)) == '*')
            {
                while ((c = fgetc(source)) != EOF)
                {
                    if (c == '*' && (c = fgetc(source)) == ')')
                        break;
                    if (c == '\n')
                        program_line++;
                }
                if (c == EOF)
                {
                    printf("ERROR %d: Lexer: comment does not terminate\n", program_line);
                    /* Change Number */ exit(1);
                }
            }
            else
            {
                ungetc(c, source);
                token = create_token(program_line, LPAREN);
                append_list(&token_list, token);
            }
            break;

        // Right Parenthesis Symbol
        case ')':
            token = create_token(program_line, RPAREN);
            append_list(&token_list, token);
            break;

        case '*':
            token = create_token(program_line, MULTI);
            append_list(&token_list, token);
            break;

        case '+':
            token = create_token(program_line, PLUS);
            append_list(&token_list, token);
            break;

        case ',':
            token = create_token(program_line, COMMA);
            append_list(&token_list, token);
            break;

        case '-':
            if ((c = fgetc(source)) == '-')
                while ((c = fgetc(source)) != '\n' && c != EOF)
                    ;
            else
            {
                token = create_token(program_line, MINUS);
                append_list(&token_list, token);
            }
            ungetc(c, source);
            break;

        // Dot Syntax
        case '.':
            token = create_token(program_line, DOT);
            append_list(&token_list, token);
            break;

        case '/':
            token = create_token(program_line, DIVISION);
            append_list(&token_list, token);
            break;

        // Integer Constant
        case '0' ... '9':
            token = create_token(program_line, INTEGER);
            append_list(&token_list, token);
            initialize_list(&new_string, sizeof(char));
            do
            {
                append_list(&new_string, (void *)(long long){c});
            } while ((c = fgetc(source)) >= '0' && c <= '9');
            append_list(&new_string, '\0');
            if (new_string.length > 10 || (new_string.length == 10 && strtol(new_string.array, NULL, 10) > INT_MAX))
            {
                printf("ERROR %d: Lexer: invalid integer %s\n", program_line, new_string.array);
            }
            ungetc(c, source);
            realloc_list(&new_string, new_string.length);
            token->token_literal = (char *)new_string.array;
            break;

        case ':':
            token = create_token(program_line, COLON);
            append_list(&token_list, token);
            break;

        case ';':
            token = create_token(program_line, SEMICOLON);
            append_list(&token_list, token);
            break;

        case '<':
            token = create_token(program_line, LTHAN);
            append_list(&token_list, token);
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
            token = create_token(program_line, EQUALS);
            append_list(&token_list, token);
            // Right Arrow Symbol or Greater Than or Equal Sign ( => )
            if ((c = fgetc(source)) == '>')
                token->type = RARROW;
            // Equals Symbol
            else
                ungetc(c, source);
            break;

        case '>':
            token = create_token(program_line, GTHAN);
            append_list(&token_list, token);
            // Greater Than or Equal Sign
            if ((c = fgetc(source)) == '=')
                token->type = GEQUAL;
            // Greater Than Sign
            else
                ungetc(c, source);
            break;

        // AT Symbol
        case '@':
            token = create_token(program_line, AT);
            append_list(&token_list, token);
            break;

        // Type Symbols
        case 'A' ... 'Z':
            token = create_token(program_line, TYPE);
            append_list(&token_list, token);
            initialize_list(&new_string, sizeof(char));
            do
            {
                append_list(&new_string, (void *)(long long){c});
            } while (((c = fgetc(source)) >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
            ungetc(c, source);
            append_list(&new_string, '\0');
            realloc_list(&new_string, new_string.length);
            token->token_literal = new_string.array;
            break;

        // Identifiers
        case 'a' ... 'z':
            token = create_token(program_line, IDENTIFIER);
            append_list(&token_list, token);
            initialize_list(&new_string, sizeof(char));
            do
            {
                append_list(&new_string, (void *)(long long){c});
            } while (((c = fgetc(source)) >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
            ungetc(c, source);
            append_list(&new_string, '\0');
            realloc_list(&new_string, new_string.length);
            token->token_literal = (char *)new_string.array;
            // Check if identifier is actually a keyword
            switch (new_string.length)
            {
            case 3:
                if (token->token_literal[0] == 'i')
                {
                    if (token->token_literal[1] == 'f')
                        token->type = IF;
                    else if (token->token_literal[1] == 'n')
                        token->type = IN;
                }
                else if (str_equal(token->token_literal, "of"))
                    token->type = OF;
                else if (str_equal(token->token_literal, "fi"))
                    token->type = FI;
                break;
            case 4:
                if (str_equal(token->token_literal, "let"))
                    token->type = LET;
                else if (token->token_literal[0] == 'n')
                {
                    if (str_equal(token->token_literal + sizeof(char), "ew"))
                        token->type = NEW;
                    else if (str_equal(token->token_literal + sizeof(char), "ot"))
                        token->type = NOT;
                }
                break;
            case 5:
                if (str_equal(token->token_literal, "case"))
                    token->type = CASE;
                else if (token->token_literal[0] == 'e')
                {
                    if (str_equal(token->token_literal + sizeof(char), "lse"))
                        token->type = ELSE;
                    else if (str_equal(token->token_literal + sizeof(char), "sac"))
                        token->type = ESAC;
                }
                else if (str_equal(token->token_literal, "loop"))
                    token->type = LOOP;
                else if (token->token_literal[0] == 't')
                {
                    if ((token->token_literal[1] == 'r' || token->token_literal[1] == 'R') &&
                        (token->token_literal[2] == 'u' || token->token_literal[2] == 'U') &&
                        (token->token_literal[3] == 'e' || token->token_literal[3] == 'E'))
                        token->type = TRUE;
                    else if (str_equal(token->token_literal + sizeof(char), "hen"))
                        token->type = THEN;
                }
                else if (str_equal(token->token_literal, "pool"))
                    token->type = POOL;
                break;
            case 6:
                if (str_equal(token->token_literal, "class"))
                    token->type = CLASS;
                else if (token->token_literal[0] == 'f' &&
                    (token->token_literal[1] == 'a' || token->token_literal[1] == 'A') &&
                    (token->token_literal[2] == 'l' || token->token_literal[2] == 'L') &&
                    (token->token_literal[3] == 's' || token->token_literal[3] == 'S') &&
                    (token->token_literal[4] == 'e' || token->token_literal[4] == 'E'))
                    token->type = FALSE;
                else if (str_equal(token->token_literal, "while"))
                    token->type = WHILE;
                break;
            case 7:
                if (str_equal(token->token_literal, "isvoid"))
                    token->type = ISVOID;
                break;
            case 9:
                if (str_equal(token->token_literal, "inherits"))
                    token->type = INHERITS;
            }

            if (token->type != IDENTIFIER)
            {
                free(token->token_literal);
                token->token_literal = NULL;
            }
            break;

        // Left Bracket Symbol
        case '{':
            token = create_token(program_line, LBRACKET);
            append_list(&token_list, token);
            break;

        // Right Bracket Symbol
        case '}':
            token = create_token(program_line, RBRACKET);
            append_list(&token_list, token);
            break;

        // Integer Complement Symbol
        case '~':
            token = create_token(program_line, INTCOMPLEMENT);
            append_list(&token_list, token);
            break;

        // Unidentified Character
        default:
            printf("ERROR: %d: Lexer: invalid character %c", program_line, c);
            /* Change Numebr */ exit(1);
        }
    }
    realloc_list(&token_list, token_list.length);
    ((struct LexicalToken **)token_list.array)[token_list.length] = NULL;
    return (struct LexicalToken **)token_list.array;
};