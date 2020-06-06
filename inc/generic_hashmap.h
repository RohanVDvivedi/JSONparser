#ifndef GENERIC_HASHMAP_H
#define GENERIC_HASHMAP_H

#include<hashmap.h>

typedef struct hashmap generic_hashmap;

generic_hashmap* get_generic_hashmap(unsigned long long int size);

const void* find_value_from_generic_hashmap(generic_hashmap* map, const void* key);

void insert_entry_in_generic_hashmap(generic_hashmap* map, const void* key, const void* value);

void for_each_entry_in_generic_hashmap(generic_hashmap* map);

print_generic_hashmap

void delete_generic_hashmap(generic_hashmap* map);

#endif