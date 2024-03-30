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

const token_type_t SINGLES_TABLE[256] = {
    ['('] = TOKEN_TYPE_OPEN_PAREN,   [')'] = TOKEN_TYPE_CLOSE_PAREN,
    ['{'] = TOKEN_TYPE_OPEN_BRACE,   ['}'] = TOKEN_TYPE_CLOSE_BRACE,
    ['['] = TOKEN_TYPE_OPEN_BRACKET, [']'] = TOKEN_TYPE_CLOSE_BRACKET,
    [';'] = TOKEN_TYPE_SEMICOLON,    [':'] = TOKEN_TYPE_COLON,
    [','] = TOKEN_TYPE_COMMA,        ['~'] = TOKEN_TYPE_BITWISE_NOT,
    ['?'] = TOKEN_TYPE_QUESTION_MARK};

const token_type_t MAYBE_DOUBLE_TABLE[256] = {
    ['!'] = TOKEN_TYPE_LOGICAL_NOT, ['+'] = TOKEN_TYPE_PLUS,
    ['-'] = TOKEN_TYPE_MINUS,       ['*'] = TOKEN_TYPE_ASTERISK,
    ['/'] = TOKEN_TYPE_DIVIDE,      ['%'] = TOKEN_TYPE_MODULUS,
    ['&'] = TOKEN_TYPE_AMPERSAND,   ['|'] = TOKEN_TYPE_BITWISE_OR,
    ['^'] = TOKEN_TYPE_BITWISE_XOR, ['='] = TOKEN_TYPE_EQUALS,
};

const token_type_t MAYBE_DOUBLE_EQUALS_TABLE[TOKEN_TYPE_MAX] = {
    [TOKEN_TYPE_EQUALS] = TOKEN_TYPE_EQUALS_EQUALS,
    [TOKEN_TYPE_LOGICAL_NOT] = TOKEN_TYPE_NOT_EQUALS,
    [TOKEN_TYPE_PLUS] = TOKEN_TYPE_PLUS_EQUALS,
    [TOKEN_TYPE_MINUS] = TOKEN_TYPE_MINUS_EQUALS,
    [TOKEN_TYPE_ASTERISK] = TOKEN_TYPE_TIMES_EQUALS,
    [TOKEN_TYPE_DIVIDE] = TOKEN_TYPE_DIVIDE_EQUALS,
    [TOKEN_TYPE_MODULUS] = TOKEN_TYPE_MODULUS_EQUALS,
    [TOKEN_TYPE_AMPERSAND] = TOKEN_TYPE_AND_EQUALS,
    [TOKEN_TYPE_BITWISE_OR] = TOKEN_TYPE_OR_EQUALS,
    [TOKEN_TYPE_BITWISE_XOR] = TOKEN_TYPE_XOR_EQUALS,
};

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

    bool found_end_of_string = false;
    bool last_char_was_backslash = false;

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
                state = LEXER_STATE_INTEGER;
                token_type = TOKEN_TYPE_INTEGER;
            } else if (lexer->current_char == '"') {
                state = LEXER_STATE_STRING;
            } else if (lexer->current_char == '\'') {
                state = LEXER_STATE_CHAR;
            } else if (SINGLES_TABLE[(size_t)lexer->current_char]) {
                state = LEXER_STATE_SINGLE;
                token_type = SINGLES_TABLE[(size_t)lexer->current_char];
            } else if (MAYBE_DOUBLE_TABLE[(size_t)lexer->current_char]) {
                state = LEXER_STATE_MAYBE_DOUBLE;
                token_type = MAYBE_DOUBLE_TABLE[(size_t)lexer->current_char];
            } else if (lexer->current_char == '<') {
                state = LEXER_STATE_MAYBE_DOUBLE_OR_TRIPLE;
                token_type = TOKEN_TYPE_LESS_THAN;
            } else if (lexer->current_char == '>') {
                state = LEXER_STATE_MAYBE_DOUBLE_OR_TRIPLE;
                token_type = TOKEN_TYPE_GREATER_THAN;
            } else if (lexer->current_char == '.') {
                state = LEXER_STATE_MAYBE_VARIADIC;
                token_type = TOKEN_TYPE_PERIOD;
            } else {
                fprintf(stderr, "error: unexpected token `%c`\n",
                        lexer->current_char);
                assert(0);
            }
        } else if (state == LEXER_STATE_SINGLE || state == LEXER_STATE_DOUBLE ||
                   state == LEXER_STATE_TRIPLE ||
                   state == LEXER_STATE_VARIADIC ||
                   state == LEXER_STATE_END_OF_FLOAT) {
            break;
        } else if (state == LEXER_STATE_MAYBE_DOUBLE) {
            // To be a double token, the second char must be one of the
            // following: [=, +, -, >, &, |]

            if (lexer->current_char == '=') {
                // The previous token is not valid combined with an =, so we
                // return the current token
                if (!MAYBE_DOUBLE_EQUALS_TABLE[token_type]) {
                    break;
                }

                token_type = MAYBE_DOUBLE_EQUALS_TABLE[token_type];
            } else if (lexer->current_char == '+') {
                if (token_type != TOKEN_TYPE_PLUS) {
                    break;
                }

                token_type = TOKEN_TYPE_INCREMENT;
            } else if (lexer->current_char == '-') {
                if (token_type != TOKEN_TYPE_MINUS) {
                    break;
                }

                token_type = TOKEN_TYPE_DECREMENT;
            } else if (lexer->current_char == '>') {
                if (token_type != TOKEN_TYPE_MINUS) {
                    break;
                }

                token_type = TOKEN_TYPE_DEREF_ACCESS;
            } else if (lexer->current_char == '&') {
                if (token_type != TOKEN_TYPE_AMPERSAND) {
                    break;
                }

                token_type = TOKEN_TYPE_LOGICAL_AND;
            } else if (lexer->current_char == '|') {
                if (token_type != TOKEN_TYPE_BITWISE_OR) {
                    break;
                }

                token_type = TOKEN_TYPE_LOGICAL_OR;
            } else {
                break;
            }

            state = LEXER_STATE_DOUBLE;
        } else if (state == LEXER_STATE_MAYBE_DOUBLE_OR_TRIPLE) {
            // Could be <, >, <=, >=, <<, >>, <<=, or >>=

            if (lexer->current_char == '=') {
                // Must be <= or =>

                if (token_type == TOKEN_TYPE_LESS_THAN) {
                    token_type = TOKEN_TYPE_LESS_THAN_OR_EQUAL;
                } else if (token_type == TOKEN_TYPE_GREATER_THAN) {
                    token_type = TOKEN_TYPE_GREATER_THAN_OR_EQUAL;
                } else {
                    assert(0);
                }

                state = LEXER_STATE_DOUBLE;
            } else if (lexer->current_char == '<' &&
                       token_type == TOKEN_TYPE_LESS_THAN) {
                // Could be << or <<=
                token_type = TOKEN_TYPE_BIT_SHIFT_LEFT;
                state = LEXER_STATE_MAYBE_TRIPLE;
            } else if (lexer->current_char == '>' &&
                       token_type == TOKEN_TYPE_GREATER_THAN) {
                // Could be >> or >>=
                token_type = TOKEN_TYPE_BIT_SHIFT_RIGHT;
                state = LEXER_STATE_MAYBE_TRIPLE;
            } else {
                // Must be < or >
                break;
            }
        } else if (state == LEXER_STATE_MAYBE_TRIPLE) {
            // Could be <<, >>, <<=, or >>=

            if (lexer->current_char != '=') {
                break;
            }

            if (token_type == TOKEN_TYPE_BIT_SHIFT_LEFT) {
                token_type = TOKEN_TYPE_SHIFT_LEFT_EQUALS;
            } else if (token_type == TOKEN_TYPE_BIT_SHIFT_RIGHT) {
                token_type = TOKEN_TYPE_SHIFT_RIGHT_EQUALS;
            } else {
                assert(0);
            }

            state = LEXER_STATE_TRIPLE;
        } else if (state == LEXER_STATE_MAYBE_VARIADIC) {
            if (lexer->current_char != '.') {
                break;
            }

            state = LEXER_STATE_REQUIRE_VARIADIC;
        } else if (state == LEXER_STATE_REQUIRE_VARIADIC) {
            if (lexer->current_char != '.') {
                fprintf(stderr,
                        "error: expected end of variadic token (`...`)\n");
                return -1;
            }

            state = LEXER_STATE_VARIADIC;
            token_type = TOKEN_TYPE_VARIADIC;
        } else if (state == LEXER_STATE_IDENTIFIER) {
            if (!is_alphanumeric_or_underscore(lexer->current_char)) {
                token_type = TOKEN_TYPE_IDENT;
                break;
            }
        } else if (state == LEXER_STATE_INTEGER) {
            // TODO: parse non-decimal bases (i.e. 0xEF and 0b11101111)

            if (lexer->current_char == '.') {
                token_type = TOKEN_TYPE_FLOAT;
                state = LEXER_STATE_FLOAT;
            } else if (!is_numeric(lexer->current_char)) {
                break;
            }
        } else if (state == LEXER_STATE_FLOAT) {
            if (lexer->current_char == 'f' || lexer->current_char == 'F') {
                state = LEXER_STATE_END_OF_FLOAT;
            } else if (!is_numeric(lexer->current_char)) {
                break;
            }
        } else if (state == LEXER_STATE_STRING) {
            if (found_end_of_string) {
                token_type = TOKEN_TYPE_STRING;
                break;
            }

            if (lexer->current_char == '"' && !last_char_was_backslash) {
                found_end_of_string = true;
            }

            last_char_was_backslash = lexer->current_char == '\\';
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