#include<json_node.h>

#include<stdlib.h>
#include<inttypes.h>

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

json_node* clone_json_node(const json_node* node_p)
{
	if(node_p == NULL)
		return NULL;
	switch(node_p->type)
	{
		case JSON_BOOL :
			return new_json_bool_node(node_p->json_bool);
		case JSON_NUM :
		{
			json_node* n = malloc(sizeof(json_node));
			n->type = JSON_NUM;
			init_copy_dstring(&(n->json_number.fraction), &(node_p->json_number.fraction));
			init_copy_dstring(&(n->json_number.exponent), &(node_p->json_number.exponent));
			return n;
		}
		case JSON_STRING :
			return new_json_string_node(&(node_p->json_string));
		case JSON_OBJECT :
		{
			json_node* n = malloc(sizeof(json_node));
			n->type = JSON_OBJECT;
			initialize_hashmap(&(n->json_object), ELEMENTS_AS_LINKEDLIST_INSERT_AT_TAIL, ((get_element_count_hashmap(&(node_p->json_object)) / 2) + 4), hash_json_object_entry, (int (*)(const void*, const void*))compare_dstring, offsetof(json_object_entry, embed_node));
			for(const json_object_entry* e = get_first_of_in_hashmap(&(node_p->json_object), ANY_IN_HASHMAP); e != NULL; e = get_next_of_in_hashmap(&(node_p->json_object), e, ANY_IN_HASHMAP))
				insert_in_json_object(n, &(e->key), clone_json_node(e->value));
			return n;
		}
		case JSON_ARRAY :
		{
			json_node* n = malloc(sizeof(json_node));
			n->type = JSON_ARRAY;
			initialize_arraylist(&(n->json_array), get_element_count_arraylist(&(node_p->json_array)));
			for(cy_uint i = 0; i < get_element_count_arraylist(&(node_p->json_array)); i++)
				push_back_to_arraylist(&(n->json_array), clone_json_node(get_nth_from_front_of_arraylist(&(node_p->json_array), i)));
			return n;
		}
	}
	return NULL;
}

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

json_node* new_json_scientific_notation_node(long double fraction, int64_t exponent)
{
	json_node* n = malloc(sizeof(json_node));
	n->type = JSON_NUM;
	init_empty_dstring(&(n->json_number.fraction), 0);
	init_empty_dstring(&(n->json_number.exponent), 0);
	snprintf_dstring(&(n->json_number.fraction), "%Lf", fraction);
	snprintf_dstring(&(n->json_number.exponent), "%" PRId64, exponent);
	return n;
}

json_node* new_json_string_node(const dstring* string_value)
{
	json_node* n = malloc(sizeof(json_node));
	n->type = JSON_STRING;
	init_copy_dstring(&(n->json_string), string_value);
	return n;
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
	if(array_node_p->type != JSON_ARRAY)
		return 0;

	if(is_full_arraylist(&(array_node_p->json_array)))
		expand_arraylist(&(array_node_p->json_array));

	return push_back_to_arraylist(&(array_node_p->json_array), node_p);
}

int delete_from_json_array(json_node* array_node_p, cy_uint index)
{
	if(array_node_p->type != JSON_ARRAY)
		return 0;

	// TODO
	int removed = 0;
	json_node* n = NULL;

	if(removed)
		delete_json_node(n);

	return removed;
}

int insert_in_json_object(json_node* object_node_p, const dstring* key, const json_node* node_p)
{
	if(object_node_p->type != JSON_OBJECT)
		return 0;

	json_object_entry* e = malloc(sizeof(json_object_entry));
	init_copy_dstring(&(e->key), key);
	e->value = (json_node*) node_p;
	initialize_llnode(&(e->embed_node));

	if((((get_element_count_hashmap(&(object_node_p->json_object)) + 1) / 2) + 4) > get_bucket_count_hashmap(&(object_node_p->json_object)))
		resize_hashmap(&(object_node_p->json_object), (((get_element_count_hashmap(&(object_node_p->json_object)) + 1) / 2) + 4));

	int inserted = insert_in_hashmap(&(object_node_p->json_object), e);
	if(!inserted)
		free(e);

	return inserted;
}

int delete_from_json_object(json_node* object_node_p, json_object_entry* entry_p)
{
	if(object_node_p->type != JSON_OBJECT)
		return 0;

	int removed = remove_from_hashmap(&(object_node_p->json_object), entry_p);

	if(removed)
	{
		deinit_dstring(&(entry_p->key));
		delete_json_node(entry_p->value);
		free(entry_p);
	}

	return removed;
}

void delete_json_node(json_node* node_p)
{
	if(node_p == NULL)
		return;
	switch(node_p->type)
	{
		case JSON_BOOL :
		{
			break;
		}
		case JSON_NUM :
		{
			deinit_dstring(&(node_p->json_number.fraction));
			deinit_dstring(&(node_p->json_number.exponent));
			break;
		}
		case JSON_STRING :
		{
			deinit_dstring(&(node_p->json_string));
			break;
		}
		case JSON_OBJECT :
		{
			arraylist temp_holder;
			initialize_arraylist(&temp_holder, get_element_count_hashmap(&(node_p->json_object)));
			for(const json_object_entry* e = get_first_of_in_hashmap(&(node_p->json_object), ANY_IN_HASHMAP); e != NULL; e = get_next_of_in_hashmap(&(node_p->json_object), e, ANY_IN_HASHMAP))
				push_back_to_arraylist(&temp_holder, e);
			deinitialize_hashmap(&(node_p->json_object));
			while(!is_empty_arraylist(&temp_holder))
			{
				json_object_entry* n = (json_object_entry*) get_front_of_arraylist(&temp_holder);
				pop_front_from_arraylist(&temp_holder);
				deinit_dstring(&(n->key));
				delete_json_node(n->value);
				free(n);
			}
			deinitialize_arraylist(&temp_holder);
		}
		case JSON_ARRAY :
		{
			while(!is_empty_arraylist(&(node_p->json_array)))
			{
				json_node* n = (json_node*) get_front_of_arraylist(&(node_p->json_array));
				pop_front_from_arraylist(&(node_p->json_array));
				delete_json_node(n);
			}
			deinitialize_arraylist(&(node_p->json_array));
		}
	}

	free(node_p);
}