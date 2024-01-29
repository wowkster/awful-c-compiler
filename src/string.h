#ifndef STRING_H
#define STRING_H

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STRING_DEFAULT_CAPACITY 64

typedef struct {
    char* buffer;
    size_t length;
    size_t capacity;
} String;

String string_create_null();
int string_create(String* string);
void string_destroy(String* string);
int string_push_char(String* string, char c);
char string_get_char(String* string, size_t index);
int string_is_empty(String* string);

#endif