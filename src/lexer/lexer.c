#include <string.h>
#include "lexer.h"
#include "array_list.h"

#define MAX_INTEGER 2147483648
#define NUMBER_OF_WHITESPACES 5

// Current Character
char c;

struct Lexeme *create_Lexeme(int program_line, enum TOKEN_NAME name)
{
    struct Lexeme *t = malloc(sizeof(struct Lexeme));
    if (t == NULL)
    {
        printf("Malloc failed to allocate space\n");
        /* Change Number */ exit(1);
    }
    t->program_line = program_line;
    t->token_literal = NULL;
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

// Creates an character array for the string lexeme
void string_constant(FILE *source, struct Lexeme *lexeme)
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
    lexeme->token_literal = (char *)string_literal->array;
    lexeme->token_name = STRING;
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
void integer_constant(FILE *source, struct Lexeme *lexeme, int *current_program_line)
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
    lexeme->token_literal = (char *)integer_constant->array;
    lexeme->token_name = INTEGER;
    free(integer_constant);
}

// Creates a character array for a type constant.
void type_constant(FILE *source, struct Lexeme *lexeme)
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
    lexeme->token_literal = type_string->array;
    lexeme->token_name = TYPE;
    free(type_string);
}

// Creates a character array for an idenitifer if its not a keyword
void identifier_constant(FILE *source, struct Lexeme *lexeme)
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
    lexeme->token_literal = (char *)identifier_string->array;
    // Check if identifier is actually a keyword
    switch (identifier_string->length)
    {
    case 3:
        if (lexeme->token_literal[0] == 'i')
        {
            if (lexeme->token_literal[1] == 'f')
                lexeme->token_name = IF;
            else if (lexeme->token_literal[1] == 'n')
                lexeme->token_name = IN;
        }
        else if (strncmp(lexeme->token_literal, "of", 2) == 0)
            lexeme->token_name = OF;
        else if (strncmp(lexeme->token_literal, "fi", 2) == 0)
            lexeme->token_name = FI;
        break;
    case 4:
        if (strncmp(lexeme->token_literal, "let", 3) == 0)
            lexeme->token_name = LET;
        else if (lexeme->token_literal[0] == 'n')
        {
            if (strncmp(&lexeme->token_literal[1], "ew", 2) == 0)
                lexeme->token_name = NEW;
            else if (strncmp(&lexeme->token_literal[1], "ot", 2) == 0)
                lexeme->token_name = NOT;
        }
        break;
    case 5:
        if (strncmp(lexeme->token_literal, "case", 4) == 0)
            lexeme->token_name = CASE;
        else if (lexeme->token_literal[0] == 'e')
        {
            if (strncmp(&lexeme->token_literal[1], "lse", 3) == 0)
                lexeme->token_name = ELSE;
            else if (strncmp(&lexeme->token_literal[1], "sac", 3) == 0)
                lexeme->token_name = ESAC;
        }
        else if (strncmp(lexeme->token_literal, "loop", 4))
            lexeme->token_name = LOOP;
        else if (lexeme->token_literal[0] == 't')
        {
            if ((lexeme->token_literal[1] == 'r' || lexeme->token_literal[1] == 'R') &&
                (lexeme->token_literal[2] == 'u' || lexeme->token_literal[2] == 'U') &&
                (lexeme->token_literal[3] == 'e' || lexeme->token_literal[3] == 'E'))
                lexeme->token_name = TRUE;
            else if (strncmp(&lexeme->token_literal[1], "hen", 3) == 0)
                lexeme->token_name = THEN;
        }
        else if (strncmp(lexeme->token_literal, "pool", 4) == 0)
            lexeme->token_name = POOL;
        break;
    case 6:
        if (strncmp(lexeme->token_literal, "class", 5) == 0)
            lexeme->token_name = CLASS;
        else if (lexeme->token_literal[0] == 'f' &&
                 (lexeme->token_literal[1] == 'a' || lexeme->token_literal[1] == 'A') &&
                 (lexeme->token_literal[2] == 'l' || lexeme->token_literal[2] == 'L') &&
                 (lexeme->token_literal[3] == 's' || lexeme->token_literal[3] == 'S') &&
                 (lexeme->token_literal[4] == 'e' || lexeme->token_literal[4] == 'E'))
            lexeme->token_name = FALSE;
        else if (strncmp(lexeme->token_literal, "while", 5) == 0)
            lexeme->token_name = WHILE;
        break;
    case 7:
        if (strncmp(lexeme->token_literal, "isvoid", 6) == 0)
            lexeme->token_name = ISVOID;
        break;
    case 9:
        if (strncmp(lexeme->token_literal, "inherits", 8) == 0)
            lexeme->token_name = INHERITS;
    }

    if (lexeme->token_name != IDENTIFIER)
    {
        free(lexeme->token_literal);
        lexeme->token_literal = NULL;
    }
    free(identifier_string);
}

struct ArrayList *lexer(FILE *source)
{
    int current_program_line;
    struct ArrayList *lexeme_list;
    struct Lexeme *lexeme;

    // Program File starts at line 1
    current_program_line = 1;
    initialize_list(lexeme_list, sizeof(struct Lexeme *));

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
        lexeme = create_Lexeme(current_program_line, 0);
        append_list(lexeme_list, lexeme);
        switch (c)
        {
        // String Constant
        case '\"':
            string_constant(source, lexeme);
            break;

        case '(':
            // Comment Case
            if ((c = fgetc(source)) == '*')
            {
                free(lexeme);
                lexeme_list->length -= 1;
                ignore_comment(source, &current_program_line);
            }
            else
            {
                lexeme->token_name = LPAREN;
                ungetc(c, source);
            }
            break;

        case ')':
            lexeme->token_name = RPAREN;
            break;

        case '*':
            lexeme->token_name = MULTI;
            break;

        case '+':
            lexeme->token_name = PLUS;
            break;

        case ',':
            lexeme->token_name = COMMA;
            break;

        case '-':
            // Inline Comment
            if ((c = fgetc(source)) == '-')
                while ((c = fgetc(source)) != '\n' && c != EOF)
                    ;
            else
                lexeme->token_name = MINUS;
            ungetc(c, source);
            break;

        case '.':
            lexeme->token_name = DOT;
            break;

        case '/':
            lexeme->token_name = DIVISION;
            break;

        // Integer Constant
        case '0' ... '9':
            integer_constant(source, lexeme, &current_program_line);
            break;

        case ':':
            lexeme->token_name = COLON;
            break;

        case ';':
            lexeme->token_name = SEMICOLON;
            break;

        case '<':
            lexeme->token_name = LTHAN;
            switch (c = fgetc(source))
            {
            // Left Arrow Symbol ( <- )
            case '-':
                lexeme->token_name = LARROW;
                break;
            // Less Than or Equal to Symbol ( <= )
            case '=':
                lexeme->token_name = LEQUAL;
                break;
            // Less Than Symbol ( < )
            default:
                lexeme->token_name = LTHAN;
                ungetc(c, source);
            }
            break;

        case '=':
            // Right Arrow Symbol or Greater Than or Equal Sign ( => )
            if ((c = fgetc(source)) == '>')
                lexeme->token_name = RARROW;
            // Equals Symbol
            else
            {
                lexeme->token_name = EQUALS;
                ungetc(c, source);
            }
            break;

        case '>':
            // Greater Than or Equal Sign
            if ((c = fgetc(source)) == '=')
                lexeme->token_name = GEQUAL;
            // Greater Than Sign
            else
            {
                lexeme->token_name = GTHAN;
                ungetc(c, source);
            }
            break;

        case '@':
            lexeme->token_name = AT;
            break;

        // Type Symbols
        case 'A' ... 'Z':
            type_constant(source, lexeme);
            break;

        // Identifiers
        case 'a' ... 'z':
            identifier_constant(source, lexeme);
            break;

        // Left Bracket Symbol
        case '{':
            lexeme->token_name = LBRACKET;
            break;

        // Right Bracket Symbol
        case '}':
            lexeme->token_name = RBRACKET;
            break;

        // Integer Complement Symbol
        case '~':
            lexeme->token_name = INTCOMPLEMENT;
            break;

        // Unidentified Character
        default:
            printf("ERROR: %d: Lexer: invalid character %c", current_program_line, c);
            /* Change Numebr */ exit(1);
        }
    }
    realloc_list(lexeme_list, lexeme_list->length);
    return lexeme_list;
};