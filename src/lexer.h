#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>
#include <stdio.h>
#include "string.h"

#define TOKEN_TYPE_EOF 0
#define TOKEN_TYPE_IDENT 1
#define TOKEN_TYPE_SEMICOLON 2
#define TOKEN_TYPE_OPEN_PAREN 3
#define TOKEN_TYPE_CLOSE_PAREN 4
#define TOKEN_TYPE_OPEN_BRACE 5
#define TOKEN_TYPE_CLOSE_BRACE 6
#define TOKEN_TYPE_INTEGER 7
#define TOKEN_TYPE_FLOAT 8

#define ENUMERATE_TOKEN_TYPES(O) \
    O(TOKEN_TYPE_EOF)            \
    O(TOKEN_TYPE_IDENT)          \
    O(TOKEN_TYPE_SEMICOLON)      \
    O(TOKEN_TYPE_OPEN_PAREN)     \
    O(TOKEN_TYPE_CLOSE_PAREN)    \
    O(TOKEN_TYPE_OPEN_BRACE)     \
    O(TOKEN_TYPE_CLOSE_BRACE)    \
    O(TOKEN_TYPE_INTEGER)        \
    O(TOKEN_TYPE_FLOAT)

typedef uint8_t token_type_t;

typedef struct {
    String value;
    uint32_t position;
    token_type_t type;
} Token;

const char* token_type_to_str(token_type_t token_type);

#define LEXER_STATE_DEFAULT 0
#define LEXER_STATE_IDENTIFIER 1
#define LEXER_STATE_NUMBER 2
#define LEXER_STATE_SINGLE 3

typedef uint8_t lexer_state_t;

typedef struct {
    FILE* source;
    uint32_t position;
    uint32_t line_number;
    char current_char;
} Lexer;

void lexer_create(Lexer* lexer, char* file_name);
void lexer_destroy(Lexer* lexer);
int lexer_next_token(Lexer* lexer, Token* token);
void lexer_create_eof_token(Lexer* lexer, Token* token);

#endif