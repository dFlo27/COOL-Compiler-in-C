#ifndef parser_h
#define parser_h

#include "lexer.h"
#include "array_list.h"

enum EXPRESSION
{
    ASSIGNMENT,
    METHODCALL,
    FUNCTIONCALL,
    IFSTATEMENT,
    WHILELOOP,
    BLOCK,
    LETBLOCK,
    CASEBLOCK,
    NEWOBJECT,
    BINARY,
    UNARY
};

struct Class
{
    struct TokenInfo *type;
    struct TokenInfo *inherits;
    struct ArrayList *features;
};

struct Feature
{
    struct TokenInfo *name;
    struct ArrayList *formals;
    struct TokenInfo *return_type;
    struct ArrayList *expressions;
};

struct Formal
{
    struct TokenInfo *identifier;
    struct TokenInfo *type;
};

struct Expression
{
    enum EXPRESSION name;
    void *data;
    int program_line;
};

struct Assignment
{
    struct TokenInfo *left_operand;
    struct Expression *right_operand;
};

struct MethodCall
{
    struct Expression *type_expression;
    struct TokenInfo *parent_class;
    struct TokenInfo *name;
    struct ArrayList *parameters;
};

struct FunctionCall
{
    struct TokenInfo *name;
    struct ArrayList *parameters;
};

struct IfStatement
{
    struct Expression *condition;
    struct Expression *true_branch;
    struct Expression *false_branch;
};

struct WhileLoop
{
    struct Expression *condition;
    struct Expression *body;
};

struct Block
{
    struct ArrayList *expression_list;
};

struct LetBlock
{
    struct ArrayList *declarations;
    struct ArrayList *assignments;
};

struct CaseBlock
{
    struct Expression *condition;
    struct ArrayList *declarations;
    struct ArrayList *expressions;
};

struct NewObject
{
    struct TokenInfo *type;
};

struct BinaryOperation
{
    struct TokenInfo *_operator;
    struct Expression *left_operand;
    struct Expression *right_operand;
};

struct UnaryOperation
{
    struct TokenInfo *_operator;
    struct Expression *expression;
};

struct ArrayList *parse(struct ArrayList *lex_list);

#endif