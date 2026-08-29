#include <stdio.h>
#include "lexer.h"

#define FLAG_COUNT 1

const char *const token_to_string[] = {
    [AT] = "AT",
    [CASE] = "CASE",
    [CLASS] = "CLASS",
    [COMMA] = "COMMA",
    [COLON] = "COLON",
    [DIVISION] = "DIVISION",
    [DOT] = "DOT",
    [EQUALS] = "EQUALS",
    [ELSE] = "ELSE",
    [ESAC] = "ESAC",
    [FALSE] = "FALSE",
    [FI] = "FI",
    [GEQUAL] = "GEQUAL",
    [GTHAN] = "GTHAN",
    [IDENTIFIER] = "IDENTIFIER",
    [IF] = "IF",
    [IN] = "IN",
    [INHERITS] = "INHERITS",
    [INTEGER] = "INTEGER",
    [INTCOMPLEMENT] = "INTCOMPLEMENT",
    [ISVOID] = "ISVOID",
    [LARROW] = "LARROW",
    [LBRACKET] = "LBRACKET",
    [LEQUAL] = "LEQUAL",
    [LET] = "LET",
    [LOOP] = "LOOP",
    [LPAREN] = "LPAREN",
    [LTHAN] = "LTHAN",
    [MINUS] = "MINUS",
    [MULTI] = "MULTI",
    [NEW] = "NEW",
    [NOT] = "NOT",
    [OF] = "OF",
    [PLUS] = "PLUS",
    [POOL] = "POOL",
    [RARROW] = "RARROW",
    [RBRACKET] = "RBRACKET",
    [RPAREN] = "RPAREN",
    [SEMICOLON] = "SEMICOLON",
    [STRING] = "STRING",
    [THEN] = "THEN",
    [TRUE] = "TRUE",
    [TYPE] = "TYPE",
    [WHILE] = "WHILE"};

enum flags
{
    NORMAL,
    OUTPUT_LEX
};

const char *string_flags[] = {
    "--lex"};

// Returns 0 if not a flag otherwise returns enum flag integer
int is_a_flag(const char *input)
{
    int i, j;
    for (i = 0; i < FLAG_COUNT; i++)
    {
        for (j = 0; input[j] == string_flags[i][j] && input[j] != '\0'; j++)
            ;
        if (input[j] == string_flags[i][j])
            return i + 1;
    }
    return NORMAL;
}

int main(int argc, char *argv[])
{
    FILE *input_file;
    FILE *output_file;
    int flag;
    if (argc < 2)
    {
        printf("No source file given\n");
        return 1;
    }
    flag = is_a_flag(argv[1]);
    input_file = fopen(argv[flag == NORMAL ? 1 : 2], "r");
    if (input_file == NULL)
    {
        perror("Source file does not exist");
        return 2;
    }
    if (fgetc(input_file) == EOF)
    {
        printf("Source file is empty\n");
        return 3;
    }
    rewind(input_file);
    struct LexicalToken **output = lex(input_file);
    if (flag == OUTPUT_LEX)
    {
        output_file = fopen("output.txt", "w");
        for (int i = 0; output[i] != NULL; i++)
        {
            fprintf(output_file, "%d\n%s\n", output[i]->program_line, token_to_string[output[i]->type]);
            if (output[i]->token_literal != NULL)
                fprintf(output_file, "%s\n", output[i]->token_literal);
        }
    }
    return 0;
}