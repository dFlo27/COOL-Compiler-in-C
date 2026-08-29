#include "arrayList.h"
#include "parser.h"

struct AST *parse(struct LexicalToken **lex_list)
{
    struct ArrayList stack;
    initialize_list(&stack, sizeof(struct AST *));
    
}