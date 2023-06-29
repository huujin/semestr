#pragma once
#include "dynamic_string.h"

dynamic_string csv_read_next(char** input);
int csv_write_entry(FILE* file, dynamic_string* s);
int csv_write_separator(FILE* file);
int csv_write_new_line(FILE* file);