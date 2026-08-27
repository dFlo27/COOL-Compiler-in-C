#ifndef ast_struct_h
#define ast_struct_h

struct AST {
    struct TOKEN *token;
    struct AST *children;
};

#endif