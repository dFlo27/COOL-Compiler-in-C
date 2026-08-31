#include <stdio.h>
#include "lexer.h"
#include "print_lex.h"

#define FLAG_COUNT 1

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
    
    struct ArrayList *lex_array = lexer(input_file);
    
    if (flag == OUTPUT_LEX)
    {
        print_lex(fopen("output.txt", "w"), lex_array);
        return 0;
    }

    return 0;
}