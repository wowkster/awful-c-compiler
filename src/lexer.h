#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "string.h"

typedef enum {
    TOKEN_TYPE_EOF,  //
    // Basic
    TOKEN_TYPE_IDENT,    // some_ident
    TOKEN_TYPE_STRING,   // "hello, world"
    TOKEN_TYPE_CHAR,     // 'a'
    TOKEN_TYPE_INTEGER,  // 123
    TOKEN_TYPE_FLOAT,    // 123.456f
    // Delimiters
    TOKEN_TYPE_OPEN_PAREN,     // (
    TOKEN_TYPE_CLOSE_PAREN,    // )
    TOKEN_TYPE_OPEN_BRACE,     // {
    TOKEN_TYPE_CLOSE_BRACE,    // }
    TOKEN_TYPE_OPEN_BRACKET,   // [
    TOKEN_TYPE_CLOSE_BRACKET,  // ]
    // Unary Ops
    TOKEN_TYPE_LOGICAL_NOT,  // !
    TOKEN_TYPE_BITWISE_NOT,  // ~
    TOKEN_TYPE_INCREMENT,    // ++
    TOKEN_TYPE_DECREMENT,    // --
    // Binary Ops - Arithmetic
    TOKEN_TYPE_PLUS,     // +
    TOKEN_TYPE_MINUS,    // -
    TOKEN_TYPE_DIVIDE,   // /
    TOKEN_TYPE_MODULUS,  // %
    // Binary Ops - Bitwise
    TOKEN_TYPE_BITWISE_OR,       // |
    TOKEN_TYPE_BITWISE_XOR,      // ^
    TOKEN_TYPE_BIT_SHIFT_LEFT,   // <<
    TOKEN_TYPE_BIT_SHIFT_RIGHT,  // >>
    // Binary Ops - Comparison
    TOKEN_TYPE_EQUALS_EQUALS,          // ==
    TOKEN_TYPE_NOT_EQUALS,             // !=
    TOKEN_TYPE_LESS_THAN,              // <
    TOKEN_TYPE_GREATER_THAN,           // >
    TOKEN_TYPE_LESS_THAN_OR_EQUAL,     // <=
    TOKEN_TYPE_GREATER_THAN_OR_EQUAL,  // >=
    // Binary Ops - Logic
    TOKEN_TYPE_LOGICAL_AND,  // &&
    TOKEN_TYPE_LOGICAL_OR,   // ||
    // Binary and Unary Ops
    TOKEN_TYPE_AMPERSAND,  // &
    TOKEN_TYPE_ASTERISK,   // *
    // Assignment
    TOKEN_TYPE_EQUALS,              // =
    TOKEN_TYPE_PLUS_EQUALS,         // +=
    TOKEN_TYPE_MINUS_EQUALS,        // -=
    TOKEN_TYPE_TIMES_EQUALS,        // *=
    TOKEN_TYPE_DIVIDE_EQUALS,       // /=
    TOKEN_TYPE_MODULUS_EQUALS,      // %=
    TOKEN_TYPE_AND_EQUALS,          // &=
    TOKEN_TYPE_OR_EQUALS,           // |=
    TOKEN_TYPE_XOR_EQUALS,          // ^=
    TOKEN_TYPE_SHIFT_LEFT_EQUALS,   // <<=
    TOKEN_TYPE_SHIFT_RIGHT_EQUALS,  // >>=
    // Other
    TOKEN_TYPE_SEMICOLON,      // ;
    TOKEN_TYPE_COLON,          // :
    TOKEN_TYPE_QUESTION_MARK,  // ?
    TOKEN_TYPE_COMMA,          // ,
    TOKEN_TYPE_PERIOD,         // .
    TOKEN_TYPE_DEREF_ACCESS,   // ->
    TOKEN_TYPE_VARIADIC,       // ...
    // End Marker
    TOKEN_TYPE_MAX
} token_type_t;

#define ENUMERATE_TOKEN_TYPES(O)        \
    O(TOKEN_TYPE_EOF)                   \
    O(TOKEN_TYPE_IDENT)                 \
    O(TOKEN_TYPE_STRING)                \
    O(TOKEN_TYPE_CHAR)                  \
    O(TOKEN_TYPE_INTEGER)               \
    O(TOKEN_TYPE_FLOAT)                 \
    O(TOKEN_TYPE_OPEN_PAREN)            \
    O(TOKEN_TYPE_CLOSE_PAREN)           \
    O(TOKEN_TYPE_OPEN_BRACE)            \
    O(TOKEN_TYPE_CLOSE_BRACE)           \
    O(TOKEN_TYPE_OPEN_BRACKET)          \
    O(TOKEN_TYPE_CLOSE_BRACKET)         \
    O(TOKEN_TYPE_LOGICAL_NOT)           \
    O(TOKEN_TYPE_BITWISE_NOT)           \
    O(TOKEN_TYPE_INCREMENT)             \
    O(TOKEN_TYPE_DECREMENT)             \
    O(TOKEN_TYPE_PLUS)                  \
    O(TOKEN_TYPE_MINUS)                 \
    O(TOKEN_TYPE_DIVIDE)                \
    O(TOKEN_TYPE_MODULUS)               \
    O(TOKEN_TYPE_BITWISE_OR)            \
    O(TOKEN_TYPE_BITWISE_XOR)           \
    O(TOKEN_TYPE_BIT_SHIFT_LEFT)        \
    O(TOKEN_TYPE_BIT_SHIFT_RIGHT)       \
    O(TOKEN_TYPE_EQUALS_EQUALS)         \
    O(TOKEN_TYPE_NOT_EQUALS)            \
    O(TOKEN_TYPE_GREATER_THAN)          \
    O(TOKEN_TYPE_LESS_THAN)             \
    O(TOKEN_TYPE_LESS_THAN_OR_EQUAL)    \
    O(TOKEN_TYPE_GREATER_THAN_OR_EQUAL) \
    O(TOKEN_TYPE_LOGICAL_AND)           \
    O(TOKEN_TYPE_LOGICAL_OR)            \
    O(TOKEN_TYPE_AMPERSAND)             \
    O(TOKEN_TYPE_ASTERISK)              \
    O(TOKEN_TYPE_EQUALS)                \
    O(TOKEN_TYPE_PLUS_EQUALS)           \
    O(TOKEN_TYPE_MINUS_EQUALS)          \
    O(TOKEN_TYPE_TIMES_EQUALS)          \
    O(TOKEN_TYPE_DIVIDE_EQUALS)         \
    O(TOKEN_TYPE_MODULUS_EQUALS)        \
    O(TOKEN_TYPE_AND_EQUALS)            \
    O(TOKEN_TYPE_OR_EQUALS)             \
    O(TOKEN_TYPE_XOR_EQUALS)            \
    O(TOKEN_TYPE_SHIFT_LEFT_EQUALS)     \
    O(TOKEN_TYPE_SHIFT_RIGHT_EQUALS)    \
    O(TOKEN_TYPE_SEMICOLON)             \
    O(TOKEN_TYPE_COLON)                 \
    O(TOKEN_TYPE_QUESTION_MARK)         \
    O(TOKEN_TYPE_COMMA)                 \
    O(TOKEN_TYPE_PERIOD)                \
    O(TOKEN_TYPE_DEREF_ACCESS)          \
    O(TOKEN_TYPE_VARIADIC)

typedef struct {
    String value;
    uint32_t position;
    token_type_t type;
} Token;

const char* token_type_to_str(token_type_t token_type);

typedef enum {
    LEXER_STATE_DEFAULT,
    LEXER_STATE_IDENTIFIER,
    LEXER_STATE_INTEGER,
    LEXER_STATE_FLOAT,
    LEXER_STATE_END_OF_FLOAT,
    LEXER_STATE_STRING,
    LEXER_STATE_CHAR,
    LEXER_STATE_SINGLE,
    LEXER_STATE_MAYBE_DOUBLE,
    LEXER_STATE_MAYBE_DOUBLE_OR_TRIPLE,
    LEXER_STATE_MAYBE_TRIPLE,
    LEXER_STATE_MAYBE_VARIADIC,
    LEXER_STATE_REQUIRE_VARIADIC,
    LEXER_STATE_DOUBLE,
    LEXER_STATE_TRIPLE,
    LEXER_STATE_VARIADIC,
} lexer_state_t;

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