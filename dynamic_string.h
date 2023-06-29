#pragma once
#include <stdio.h>

typedef struct {
    char* buffer;
    size_t length;
    size_t capacity;
} dynamic_string;

void dynamic_string_init(dynamic_string* s);
void dynamic_string_free(dynamic_string* s);
int dynamic_string_push_back(dynamic_string* s, char item);
size_t dynamic_string_get_hash(dynamic_string* s);
dynamic_string* dynamic_string_copy(dynamic_string* s);