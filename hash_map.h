#pragma once
#include "dynamic_string.h"

typedef struct {
	dynamic_string* key;
	void* value;
} hash_map_entry;

typedef struct {
	hash_map_entry* entries;
	size_t length;
	size_t capacity;
	void (*free_value)(void*);
} hash_map;

void hash_map_init(hash_map* map, void (*free_value)(void*));
int hash_map_add(hash_map* map, dynamic_string* key, void* value);
void hash_map_free(hash_map* map);
hash_map_entry* hash_map_find(hash_map* map, dynamic_string* key);
void hash_map_delete(hash_map* map, hash_map_entry* entry);