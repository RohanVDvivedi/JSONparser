#include<json_node.h>

#include<stdlib.h>
#include<inttypes.h>

json_node* new_json_bool_node(int bool_value)
{
	json_node* n = malloc(sizeof(json_node));
	n->type = JSON_BOOL;
	n->json_bool = bool_value;
	return n;
}

json_node* new_json_unsigned_integer_node(uint64_t num_value)
{
	json_node* n = malloc(sizeof(json_node));
	n->type = JSON_NUM;
	init_empty_dstring(&(n->json_number.fraction), 0);
	init_empty_dstring(&(n->json_number.exponent), 0);
	snprintf_dstring(&(n->json_number.fraction), "%" PRIu64, num_value);
	return n;
}

json_node* new_json_integer_node(int64_t num_value)
{
	json_node* n = malloc(sizeof(json_node));
	n->type = JSON_NUM;
	init_empty_dstring(&(n->json_number.fraction), 0);
	init_empty_dstring(&(n->json_number.exponent), 0);
	snprintf_dstring(&(n->json_number.fraction), "%" PRId64, num_value);
	return n;
}

json_node* new_json_float_node(long double num_value)
{
	json_node* n = malloc(sizeof(json_node));
	n->type = JSON_NUM;
	init_empty_dstring(&(n->json_number.fraction), 0);
	init_empty_dstring(&(n->json_number.exponent), 0);
	snprintf_dstring(&(n->json_number.fraction), "%Lf", num_value);
	return n;
}

json_node* new_json_string_node(const dstring* string_value)
{
	json_node* n = malloc(sizeof(json_node));
	n->type = JSON_STRING;
	init_copy_dstring(&(n->json_number.exponent), string_value);
	return n;
}

static cy_uint hash_json_object_entry(const void* ent_p)
{
	const json_object_entry* e = ent_p;
	cy_uint hash = 0;
	const char* key_data = get_byte_array_dstring(&(e->key));
	cy_uint key_size = get_char_count_dstring(&(e->key));
	for(cy_uint i = 0; i < key_size; i++)
		hash = hash ^ (CY_UINT_C(13) * (i + 1) * ((cy_uint)key_data[i]));
	return hash;
}

json_node* new_json_object_node(cy_uint element_count, const json_object_entry entries[])
{
	json_node* n = malloc(sizeof(json_node));
	n->type = JSON_OBJECT;
	initialize_hashmap(&(n->json_object), ELEMENTS_AS_LINKEDLIST_INSERT_AT_TAIL, ((element_count / 2) + 4), hash_json_object_entry, (int (*)(const void*, const void*))compare_dstring, offsetof(json_object_entry, embed_node));
	for(cy_uint i = 0; i < element_count; i++)
		insert_in_json_object(n, &(entries[i].key), entries[i].value);
	return n;
}

json_node* new_json_array_node(cy_uint element_count, const json_node* elements[])
{
	json_node* n = malloc(sizeof(json_node));
	n->type = JSON_ARRAY;
	initialize_arraylist(&(n->json_array), element_count);
	for(cy_uint i = 0; i < element_count; i++)
		append_to_json_array(n, elements[i]);
	return n;
}

int append_to_json_array(json_node* array_node_p, const json_node* node_p)
{

}

int delete_from_jsom_array(json_node* array_node_p, cy_uint index);

int insert_in_json_object(json_node* object_node_p, const dstring* key, const json_node* node_p);

int delete_from_json_object(json_node* object_node_p, json_object_entry* entry_p);

void delete_json_node(json_node* node_p);