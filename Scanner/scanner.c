#include <stdio.h>
#include <lexer.h>

int main(int argc, char *argv[]) {
    FILE *input_file;
    if (argc < 2) {
        printf("No source file given\n");
        return 1;
    }
    input_file = fopen(argv[1], "r");
    if (input_file == NULL) {
        perror("Source file does not exist");
        return 2;
    }
    if (fgetc(input_file) == EOF) {
        printf("Source file is empty\n");
        return 3;
    }
    rewind(input_file);
    lex(input_file);
    return 0;
}