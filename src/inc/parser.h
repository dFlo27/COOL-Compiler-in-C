#ifndef parser_h
#define parser_h

#include "lexer.h"
#include "array_list.h"

enum EXPR_NAME
{
    ASSIGNMENT,
    METHODCALL,
    FUNCTIONCALL,
    IFSTATEMENT
};

struct Class
{
    char *class_type;
    char *inherits_from;
    struct ArrayList *features;
    int program_line;
};

struct Features
{
    char *feature_name;
    struct ArrayList *formals;
    char *return_type;
    struct ArrayList *expressions;
    int program_line;
};

struct Formal
{
    char *identifier;
    char *formal_type;
    int program_line;
};

struct Expression
{
    program_line;
    enum EXPR_NAME name;
    void *data;
};

struct Assignment
{
    char *left_operand;
    struct Expression *right_operand;
};

struct MethodCall
{
    struct Expression *class_expression;
    char *parent_class;
    char *method_name;
    struct ArrayList *parameters;
};

struct FunctionCall
{
    char *function_name;
    struct ArrayList *parameters
};

struct IfStatement
{
    struct Expression *condition;
    struct Expression *
};

struct ArrayList *parse(struct ArrayList *);

#endif