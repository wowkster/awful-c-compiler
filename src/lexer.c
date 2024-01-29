#include "lexer.h"
#include "util.h"

const char* token_type_to_str(token_type_t token_type) {
#define TOKEN_TYPE_TO_STR(TOKEN_TYPE) \
    case TOKEN_TYPE:                  \
        return #TOKEN_TYPE;

    switch (token_type) {
        ENUMERATE_TOKEN_TYPES(TOKEN_TYPE_TO_STR)
        default:
            assert(0);
    }
#undef TOKEN_TYPE_TO_STR
}

void lexer_create(Lexer* lexer, char* file_name) {
    FILE* fp = fopen(file_name, "r");

    lexer->source = fp;
    lexer->position = 0;
    lexer->line_number = 1;
    lexer->current_char = 0;
}

void lexer_destroy(Lexer* lexer) {
    fclose(lexer->source);
}

int lexer_next_token(Lexer* lexer, Token* token) {
    if (feof(lexer->source)) {
        lexer_create_eof_token(lexer, token);
        return 0;
    }

    lexer_state_t state = LEXER_STATE_DEFAULT;
    size_t token_start = lexer->position;
    token_type_t token_type;
    String token_value;
    string_create(&token_value);

    if (!lexer->current_char) {
        lexer->current_char = fgetc(lexer->source);
    }

    // Three possible outcomes from this logic:
    //   1. Fall through to next loop (pushes current char to token string and
    //   keeps looking)
    //   2. goto ignore_current_char (ignores current char and keeps looking)
    //   3. Break early (returns the current token as completed without the
    //   current char)
    while (!feof(lexer->source)) {
        if (lexer->current_char == '\n') {
            lexer->line_number++;
        }

        if (state == LEXER_STATE_DEFAULT) {
            if (is_whitespace(lexer->current_char)) {
                goto ignore_current_char;
            } else if (is_alpha_or_underscore(lexer->current_char)) {
                state = LEXER_STATE_IDENTIFIER;
            } else if (is_numeric(lexer->current_char)) {
                state = LEXER_STATE_NUMBER;
                token_type = TOKEN_TYPE_INTEGER;
            } else if (lexer->current_char == '(') {
                state = LEXER_STATE_SINGLE;
                token_type = TOKEN_TYPE_OPEN_PAREN;
            } else if (lexer->current_char == ')') {
                state = LEXER_STATE_SINGLE;
                token_type = TOKEN_TYPE_CLOSE_PAREN;
            } else if (lexer->current_char == '{') {
                state = LEXER_STATE_SINGLE;
                token_type = TOKEN_TYPE_OPEN_BRACE;
            } else if (lexer->current_char == '}') {
                state = LEXER_STATE_SINGLE;
                token_type = TOKEN_TYPE_CLOSE_BRACE;
            }else if (lexer->current_char == ';') {
                state = LEXER_STATE_SINGLE;
                token_type = TOKEN_TYPE_SEMICOLON;
            }
        } else if (state == LEXER_STATE_SINGLE) {
            break;
        } else if (state == LEXER_STATE_IDENTIFIER) {
            if (!is_alphanumeric_or_underscore(lexer->current_char)) {
                token_type = TOKEN_TYPE_IDENT;
                break;
            }
        } else if (state == LEXER_STATE_NUMBER) {
            if (lexer->current_char == '.') {
                token_type = TOKEN_TYPE_FLOAT;
            } else if (!is_numeric(lexer->current_char)) {
                break;
            }
        } else {
            fprintf(stderr, "error: unexpected lexer state (%d)\n", state);
            assert(0);
        }

        if (string_push_char(&token_value, lexer->current_char)) {
            fprintf(stderr, "error: could not push char to token string\n");
            return -1;
        }

    ignore_current_char:
        lexer->current_char = fgetc(lexer->source);
        lexer->position++;
    }

    if (!string_is_empty(&token_value)) {
        token->value = token_value;
        token->position = token_start;
        token->type = token_type;
    } else {
        string_destroy(&token_value);
        lexer_create_eof_token(lexer, token);
    }

    return 0;
}

void lexer_create_eof_token(Lexer* lexer, Token* token) {
    memset(token, 0, sizeof(Token));
    token->type = TOKEN_TYPE_EOF;
    token->position = lexer->position;
}