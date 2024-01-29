int is_whitespace(char c) {
    return c == ' ' || c == '\n' || c == '\t' || c == '\v';
}

int is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_numeric(char c) {
    return (c >= '0' && c <= '9');
}

int is_alpha_or_underscore(char c) {
    return is_alpha(c) || c == '_';
}

int is_alphanumeric_or_underscore(char c) {
    return is_alpha(c) || is_numeric(c) || c == '_';
}
