#include <string.h>
#include "lexer.h"
#include "array_list.h"

#define MAX_INTEGER 2147483648
#define NUMBER_OF_WHITESPACES 5

// Current Character
char c;

struct TokenInfo *create_token(int program_line, enum TOKEN name)
{
    struct TokenInfo *t = malloc(sizeof(struct TokenInfo));
    if (t == NULL)
    {
        printf("Malloc failed to allocate space\n");
        /* Change Number */ exit(1);
    }
    t->program_line = program_line;
    t->lexeme = NULL;
    t->token_name = name;
    return t;
}

char whitespaces[] = {'\t', '\v', '\f', '\r', ' '};

int is_whitespace(char c)
{
    int i;
    for (i = 0; i < NUMBER_OF_WHITESPACES && c != whitespaces[i]; i++)
        ;
    return i > NUMBER_OF_WHITESPACES;
}

// Creates an character array for the string token
void string_constant(FILE *source, struct TokenInfo *token)
{
    struct ArrayList *string_literal;
    initialize_list(string_literal, sizeof(char));

    while ((c = fgetc(source)) != '\"')
    {
        append_list(string_literal, (void *)(long long){c});
        switch (c)
        {
        case '\0':
            printf("ERROR %d: Lexer: String contains NULL character\n");
            /* Change Number */ exit(1);
            break;
        case '\n':
        case EOF:
            printf("ERROR %d: Lexer: String is missing a quotation mark\n");
            /* Change Number */ exit(1);
            break;
        case '\\':
            c = fgetc(source);
            if (c == '\"')
                append_list(string_literal, (void *)(long long){c});
            else
                ungetc(c, source);
            break;
        }
    }
    append_list(string_literal, '\0');
    realloc_list(string_literal, string_literal->length);
    token->lexeme = (char *)string_literal->array;
    token->token_name = STRING;
    free(string_literal);
}

// Ignores the multi line comment that starts with (* and end with *).
// Returns an error if comment does not end.
void ignore_comment(FILE *source, int *current_program_line)
{
    while ((c = fgetc(source)) != EOF)
    {
        if (c == '*' && (c = fgetc(source)) == ')')
            break;
        if (c == '\n')
            (*current_program_line)++;
    }
    if (c == EOF)
    {
        printf("ERROR %d: Lexer: comment does not terminate\n", *current_program_line);
        /* Change Number */ exit(1);
    }
}

// Creates a character array for the integer constant. Must be less than MAX_INTEGER
// MAX_INTEGER is equivalent to the absolute value of the minimum negative integer because 
// the lexer can't tell if the integer is negative or not
void integer_constant(FILE *source, struct TokenInfo *token, int *current_program_line)
{
    struct ArrayList *integer_constant;
    initialize_list(integer_constant, sizeof(char));
    do
    {
        append_list(integer_constant, (void *)(long long){c});
    } while ((c = fgetc(source)) >= '0' && c <= '9');
    append_list(integer_constant, '\0');
    if (integer_constant->length > 10 || (integer_constant->length == 10 && strtol(integer_constant->array, NULL, 10) > MAX_INTEGER))
    {
        printf("ERROR %d: Lexer: invalid integer %s\n", current_program_line, integer_constant->array);
    }
    ungetc(c, source);
    realloc_list(integer_constant, integer_constant->length);
    token->lexeme = (char *)integer_constant->array;
    token->token_name = INTEGER;
    free(integer_constant);
}

// Creates a character array for a type constant.
void type_constant(FILE *source, struct TokenInfo *token)
{
    struct ArrayList *type_string;
    initialize_list(type_string, sizeof(char));
    do
    {
        append_list(type_string, (void *)(long long){c});
    } while (((c = fgetc(source)) >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
    ungetc(c, source);
    append_list(type_string, '\0');
    realloc_list(type_string, type_string->length);
    token->lexeme = type_string->array;
    token->token_name = TYPE;
    free(type_string);
}

// Creates a character array for an idenitifer if its not a keyword
void identifier_constant(FILE *source, struct TokenInfo *token)
{
    struct ArrayList *identifier_string;
    initialize_list(identifier_string, sizeof(char));
    do
    {
        append_list(identifier_string, (void *)(long long){c});
    } while (((c = fgetc(source)) >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_');
    ungetc(c, source);
    append_list(identifier_string, '\0');
    realloc_list(identifier_string, identifier_string->length);
    token->lexeme = (char *)identifier_string->array;
    // Check if identifier is actually a keyword
    switch (identifier_string->length)
    {
    case 3:
        if (token->lexeme[0] == 'i')
        {
            if (token->lexeme[1] == 'f')
                token->token_name = IF;
            else if (token->lexeme[1] == 'n')
                token->token_name = IN;
        }
        else if (strncmp(token->lexeme, "of", 2) == 0)
            token->token_name = OF;
        else if (strncmp(token->lexeme, "fi", 2) == 0)
            token->token_name = FI;
        break;
    case 4:
        if (strncmp(token->lexeme, "let", 3) == 0)
            token->token_name = LET;
        else if (token->lexeme[0] == 'n')
        {
            if (strncmp(&token->lexeme[1], "ew", 2) == 0)
                token->token_name = NEW;
            else if (strncmp(&token->lexeme[1], "ot", 2) == 0)
                token->token_name = NOT;
        }
        break;
    case 5:
        if (strncmp(token->lexeme, "case", 4) == 0)
            token->token_name = CASE;
        else if (token->lexeme[0] == 'e')
        {
            if (strncmp(&token->lexeme[1], "lse", 3) == 0)
                token->token_name = ELSE;
            else if (strncmp(&token->lexeme[1], "sac", 3) == 0)
                token->token_name = ESAC;
        }
        else if (strncmp(token->lexeme, "loop", 4))
            token->token_name = LOOP;
        else if (token->lexeme[0] == 't')
        {
            if ((token->lexeme[1] == 'r' || token->lexeme[1] == 'R') &&
                (token->lexeme[2] == 'u' || token->lexeme[2] == 'U') &&
                (token->lexeme[3] == 'e' || token->lexeme[3] == 'E'))
                token->token_name = TRUE;
            else if (strncmp(&token->lexeme[1], "hen", 3) == 0)
                token->token_name = THEN;
        }
        else if (strncmp(token->lexeme, "pool", 4) == 0)
            token->token_name = POOL;
        break;
    case 6:
        if (strncmp(token->lexeme, "class", 5) == 0)
            token->token_name = CLASS;
        else if (token->lexeme[0] == 'f' &&
                 (token->lexeme[1] == 'a' || token->lexeme[1] == 'A') &&
                 (token->lexeme[2] == 'l' || token->lexeme[2] == 'L') &&
                 (token->lexeme[3] == 's' || token->lexeme[3] == 'S') &&
                 (token->lexeme[4] == 'e' || token->lexeme[4] == 'E'))
            token->token_name = FALSE;
        else if (strncmp(token->lexeme, "while", 5) == 0)
            token->token_name = WHILE;
        break;
    case 7:
        if (strncmp(token->lexeme, "isvoid", 6) == 0)
            token->token_name = ISVOID;
        break;
    case 9:
        if (strncmp(token->lexeme, "inherits", 8) == 0)
            token->token_name = INHERITS;
    }

    if (token->token_name != IDENTIFIER)
    {
        free(token->lexeme);
        token->lexeme = NULL;
    }
    free(identifier_string);
}

struct ArrayList *lexer(FILE *source)
{
    int current_program_line;
    struct ArrayList *token_list;
    struct TokenInfo *token;

    // Program File starts at line 1
    current_program_line = 1;
    initialize_list(token_list, sizeof(struct TokenInfo *));

    while ((c = fgetc(source)) != EOF)
    {
        // Inline Whitespace
        if (is_whitespace(c))
            continue;
        // New Line Case
        if (c == '\n')
        {
            current_program_line++;
            continue;
        }
        token = create_token(current_program_line, 0);
        append_list(token_list, token);
        switch (c)
        {
        // String Constant
        case '\"':
            string_constant(source, token);
            break;

        case '(':
            // Comment Case
            if ((c = fgetc(source)) == '*')
            {
                free(token);
                token_list->length -= 1;
                ignore_comment(source, &current_program_line);
            }
            else
            {
                token->token_name = LPAREN;
                ungetc(c, source);
            }
            break;

        case ')':
            token->token_name = RPAREN;
            break;

        case '*':
            token->token_name = MULTI;
            break;

        case '+':
            token->token_name = PLUS;
            break;

        case ',':
            token->token_name = COMMA;
            break;

        case '-':
            // Inline Comment
            if ((c = fgetc(source)) == '-')
                while ((c = fgetc(source)) != '\n' && c != EOF)
                    ;
            else
                token->token_name = MINUS;
            ungetc(c, source);
            break;

        case '.':
            token->token_name = DOT;
            break;

        case '/':
            token->token_name = DIVISION;
            break;

        // Integer Constant
        case '0' ... '9':
            integer_constant(source, token, &current_program_line);
            break;

        case ':':
            token->token_name = COLON;
            break;

        case ';':
            token->token_name = SEMICOLON;
            break;

        case '<':
            token->token_name = LTHAN;
            switch (c = fgetc(source))
            {
            // Left Arrow Symbol ( <- )
            case '-':
                token->token_name = LARROW;
                break;
            // Less Than or Equal to Symbol ( <= )
            case '=':
                token->token_name = LEQUAL;
                break;
            // Less Than Symbol ( < )
            default:
                token->token_name = LTHAN;
                ungetc(c, source);
            }
            break;

        case '=':
            // Right Arrow Symbol or Greater Than or Equal Sign ( => )
            if ((c = fgetc(source)) == '>')
                token->token_name = RARROW;
            // Equals Symbol
            else
            {
                token->token_name = EQUALS;
                ungetc(c, source);
            }
            break;

        case '>':
            // Greater Than or Equal Sign
            if ((c = fgetc(source)) == '=')
                token->token_name = GEQUAL;
            // Greater Than Sign
            else
            {
                token->token_name = GTHAN;
                ungetc(c, source);
            }
            break;

        case '@':
            token->token_name = AT;
            break;

        // Type Symbols
        case 'A' ... 'Z':
            type_constant(source, token);
            break;

        // Identifiers
        case 'a' ... 'z':
            identifier_constant(source, token);
            break;

        // Left Bracket Symbol
        case '{':
            token->token_name = LBRACKET;
            break;

        // Right Bracket Symbol
        case '}':
            token->token_name = RBRACKET;
            break;

        // Integer Complement Symbol
        case '~':
            token->token_name = INTCOMPLEMENT;
            break;

        // Unidentified Character
        default:
            printf("ERROR: %d: Lexer: invalid character %c", current_program_line, c);
            /* Change Numebr */ exit(1);
        }
    }
    realloc_list(token_list, token_list->length);
    return token_list;
};