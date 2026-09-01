#include "parser.h"

int lex_ptr = 0;
struct ArrayList *stack;
struct ArrayList *classes;

struct TokenInfo *get_next_token(struct ArrayList *token_list)
{
    if (lex_ptr < token_list->length)
        return ((struct TokenInfo **)token_list->array)[lex_ptr++];
    return NULL;
}

void read_feature(struct ArrayList *token_list, struct ArrayList *features)
{
    struct Token *token;
}

void read_class(struct ArrayList *token_list, struct TokenInfo *token)
{
    struct Class *new_class;

    new_class = malloc(sizeof(struct Class));
    initialize_list(new_class->features, sizeof(struct Feature *));
    
    append_list(classes, new_class);

    if (token->lexeme != CLASS)
    {
        printf("ERROR %d: Parser: Class declaration is missing\n", token->program_line);
        /* Change Number */ exit(1);
    }
    new_class->type = token;

    token = get_next_token(token_list);

    // If class explicitly inherits
    if (token != NULL && token->lexeme == INHERITS)
    {
        // Free Inherits Token
        free(token);
        token = get_next_token(token_list);
        if (token != NULL && token->token_name == TYPE)
            new_class->inherits = token;
    }
    else
        new_class->inherits = NULL;

    while ((token = get_next_token(token_list)) != NULL && token->token_name != LBRACKET)
        read_feature(token_list, new_class->features);

    if (token == NULL)
    {
        printf("ERROR %d: Parser: Class %s declaration is incomplete\n", new_class->type->program_line, new_class->type->lexeme);
        /* Change Number */ exit(1);
    }

    // Free Left Bracket Token
    free(token);

    initialize_list(new_class->features, sizeof(struct Feature *));
    do
    {
        read_feature(token_list, new_class->features);
        token = get_next_token(token_list);
    } while (token->token_name == SEMICOLON);
    if (token == NULL || token->token_name != RBRACKET)
    {
        printf("ERROR %d: Parser: Class is improperly closed\n", (new_class->features->length > 0) ? ((struct Feature **) new_class->features)[new_class->features->length - 1] : new_class->type->program_line);
    }
}

struct ArrayList *parse(struct ArrayList *token_list)
{
    initialize_list(stack, sizeof(struct TokenInfo *));
    initialize_list(classes, sizeof(struct Class *));
    struct TokenInfo *token;

    while ((token = get_next_token(token_list)) != NULL)
        read_class(token_list, token);

    free(token_list);
    return classes;
}