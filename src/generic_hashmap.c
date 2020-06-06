#include<generic_hashmap.h>

typedef struct bucket bucket;
struct bucket
{
	void* key;
	void* value;

	// used for resolving collisions
	llnode generic_hashmap_node;
};

generic_hashmap* get_generic_hashmap(unsigned long long int size)
{
	generic_hashmap* map 
	return;
}

const void* find_value_from_generic_hashmap(const generic_hashmap* map, const void* key);
{

}

void insert_entry_in_generic_hashmap(generic_hashmap* map, const void* key, const void* value);
{

}

void delete_generic_hashmap(generic_hashmap* map)
{
	free(map);
}