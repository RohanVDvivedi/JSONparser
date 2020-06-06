#ifndef GENERIC_HASHMAP_H
#define GENERIC_HASHMAP_H

#include<hashmap.h>

typedef struct hashmap generic_hashmap;

generic_hashmap* get_generic_hashmap(unsigned long long int size, unsigned long long int(*key_hashing_function)(const void*), int(*compare_function)(const void*, const void*));

const void* find_value_from_generic_hashmap(generic_hashmap* map, const void* key);

void insert_entry_in_generic_hashmap(generic_hashmap* map, const void* key, const void* value);

void for_each_entry_in_generic_hashmap(generic_hashmap* map, void (*operation)(const void* key, const void* value, const void* additonal_params), const void* additional_params);

void print_generic_hashmap(generic_hashmap* map, void (*print_key)(const void*), void (*print_value)(const void*));

void delete_generic_hashmap(generic_hashmap* map);

#endif