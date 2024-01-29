#include "string.h"

String string_create_null() {
    String string = {0};
    return string;
}

int string_create(String* string) {
    string->length = 0;
    string->capacity = STRING_DEFAULT_CAPACITY;
    string->buffer = (char*)malloc(string->capacity);

    if (!string->buffer) {
        return -1;
    }

    memset(string->buffer, 0, string->capacity);

    return 0;
}

void string_destroy(String* string) {
    free(string->buffer);

    string->capacity = 0;
    string->length = 0;
}

int string_resize(String* string) {
    string->capacity *= 2;
    string->buffer = reallocf(string->buffer, string->capacity);

    if (!string->buffer) {
        return -1;
    }

    memset(string->buffer + string->length, 0,
           string->capacity - string->length);

    return 0;
}

int string_push_char(String* string, char c) {
    // Reallocate if at max capacity
    if (string->length >= string->capacity - 1) {
        if (string_resize(string)) {
            return -1;
        }
    }

    // Insert new value
    string->buffer[string->length] = c;
    string->length++;
    return 0;
}

char string_get_char(String* string, size_t index) {
    assert(index < string->length);

    return string->buffer[index];
}

int string_is_empty(String* string) {
    return string->length == 0;
}