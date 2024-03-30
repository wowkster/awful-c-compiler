#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

void print_usage() {
    printf("OVERVIEW: awful C compiler\n");
    printf("\n");
    printf("USAGE: acc [options] file...\n");
    printf("\n");
    printf("OPTIONS:\n");
    printf("  none yet...\n");
    exit(0);
}

int main(int argc, char** argv) {
    if (argc == 1) {
        printf("error: no input files\n");
        exit(1);
    }

    char* filename = argv[1];

    Lexer lexer;
    lexer_create(&lexer, filename);

    Token token;

    do {
        if (lexer_next_token(&lexer, &token)) {
            printf("error: no tokens!\n");
            exit(1);
        }

        printf("Token: ");

        if (token.type != TOKEN_TYPE_EOF) {
            printf("%-32s %s\n", token_type_to_str(token.type),
                   token.value.buffer);
        } else {
            printf("EOF\n");
        }
    } while (token.type != TOKEN_TYPE_EOF);
}