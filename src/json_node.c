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
			return new_json_decimal_string_scientific_notation_node(&(node_p->json_number.fraction), &(node_p->json_number.exponent));
		case JSON_STRING :
			return new_json_string_node(&(node_p->json_string));
		case JSON_OBJECT :
		{
			json_node* n = malloc(sizeof(json_node));
			if(n == NULL)
				return NULL;
			n->type = JSON_OBJECT;
			if(!initialize_hashmap(&(n->json_object), ROBINHOOD_HASHING, get_element_count_hashmap(&(node_p->json_object)) + 4, &simple_hasher(hash_json_object_entry), &simple_comparator(((int (*)(const void*, const void*))compare_dstring)), offsetof(json_object_entry, embed_node)))
			{
				free(n);
				return NULL;
			}
			for(const json_object_entry* e = get_first_of_in_hashmap(&(node_p->json_object), ANY_IN_HASHMAP); e != NULL; e = get_next_of_in_hashmap(&(node_p->json_object), e, ANY_IN_HASHMAP))
			{
				json_node* node_p_child = e->value;
				json_node* n_child = clone_json_node(node_p_child);
				if(node_p_child != NULL && n_child == NULL)
				{
					delete_json_node(n);
					return NULL;
				}
				if(!insert_in_json_object(n, &(e->key), n_child))
				{
					delete_json_node(n_child);
					delete_json_node(n);
					return NULL;
				}
			}
			return n;
		}
		case JSON_ARRAY :
		{
			json_node* n = malloc(sizeof(json_node));
			if(n == NULL)
				return NULL;
			n->type = JSON_ARRAY;
			if(!initialize_arraylist(&(n->json_array), get_element_count_arraylist(&(node_p->json_array))))
			{
				free(n);
				return NULL;
			}
			for(cy_uint i = 0; i < get_element_count_arraylist(&(node_p->json_array)); i++)
			{
				json_node* node_p_child = (json_node*) get_from_front_of_arraylist(&(node_p->json_array), i);
				json_node* n_child = clone_json_node(node_p_child);
				if(node_p_child != NULL && n_child == NULL)
				{
					delete_json_node(n);
					return NULL;
				}
				if(!push_back_to_arraylist(&(n->json_array), n_child))
				{
					delete_json_node(n_child);
					delete_json_node(n);
					return NULL;
				}
			}
			return n;
		}
	}
	return NULL;
}

const json_node true_json_node = {.type = JSON_BOOL, .json_bool = 1};
const json_node false_json_node = {.type = JSON_BOOL, .json_bool = 0};

json_node* new_json_bool_node(int bool_value)
{
	return (json_node*)(bool_value ? (&true_json_node) : (&false_json_node));
}

json_node* new_json_unsigned_integer_node(uint64_t num_value)
{
	json_node* n = malloc(sizeof(json_node));
	if(n == NULL)
		return NULL;
	n->type = JSON_NUM;
	if(!init_empty_dstring(&(n->json_number.fraction), 0))
	{
		free(n);
		return NULL;
	}
	if(!init_empty_dstring(&(n->json_number.exponent), 0))
	{
		deinit_dstring(&(n->json_number.fraction));
		free(n);
		return NULL;
	}
	if(!snprintf_dstring(&(n->json_number.fraction), "%" PRIu64, num_value))
	{
		deinit_dstring(&(n->json_number.fraction));
		deinit_dstring(&(n->json_number.exponent));
		free(n);
		return NULL;
	}
	return n;
}

json_node* new_json_integer_node(int64_t num_value)
{
	json_node* n = malloc(sizeof(json_node));
	if(n == NULL)
		return NULL;
	n->type = JSON_NUM;
	if(!init_empty_dstring(&(n->json_number.fraction), 0))
	{
		free(n);
		return NULL;
	}
	if(!init_empty_dstring(&(n->json_number.exponent), 0))
	{
		deinit_dstring(&(n->json_number.fraction));
		free(n);
		return NULL;
	}
	if(!snprintf_dstring(&(n->json_number.fraction), "%" PRId64, num_value))
	{
		deinit_dstring(&(n->json_number.fraction));
		deinit_dstring(&(n->json_number.exponent));
		free(n);
		return NULL;
	}
	return n;
}

json_node* new_json_float_node(long double num_value)
{
	json_node* n = malloc(sizeof(json_node));
	if(n == NULL)
		return NULL;
	n->type = JSON_NUM;
	if(!init_empty_dstring(&(n->json_number.fraction), 0))
	{
		free(n);
		return NULL;
	}
	if(!init_empty_dstring(&(n->json_number.exponent), 0))
	{
		deinit_dstring(&(n->json_number.fraction));
		free(n);
		return NULL;
	}
	if(!snprintf_dstring(&(n->json_number.fraction), "%Lf", num_value))
	{
		deinit_dstring(&(n->json_number.fraction));
		deinit_dstring(&(n->json_number.exponent));
		free(n);
		return NULL;
	}
	return n;
}

json_node* new_json_scientific_notation_node(long double fraction, int64_t exponent)
{
	json_node* n = malloc(sizeof(json_node));
	if(n == NULL)
		return NULL;
	n->type = JSON_NUM;
	if(!init_empty_dstring(&(n->json_number.fraction), 0))
	{
		free(n);
		return NULL;
	}
	if(!init_empty_dstring(&(n->json_number.exponent), 0))
	{
		deinit_dstring(&(n->json_number.fraction));
		free(n);
		return NULL;
	}
	if(!snprintf_dstring(&(n->json_number.fraction), "%Lf", fraction))
	{
		deinit_dstring(&(n->json_number.fraction));
		deinit_dstring(&(n->json_number.exponent));
		free(n);
		return NULL;
	}
	if(!snprintf_dstring(&(n->json_number.exponent), "%" PRId64, exponent))
	{
		deinit_dstring(&(n->json_number.fraction));
		deinit_dstring(&(n->json_number.exponent));
		free(n);
		return NULL;
	}
	return n;
}

json_node* new_json_decimal_string_scientific_notation_node(const dstring* fraction, const dstring* exponent)
{
	json_node* n = malloc(sizeof(json_node));
	if(n == NULL)
		return NULL;
	n->type = JSON_NUM;
	if(!init_copy_dstring(&(n->json_number.fraction), fraction))
	{
		free(n);
		return NULL;
	}
	if(!init_copy_dstring(&(n->json_number.exponent), exponent))
	{
		deinit_dstring(&(n->json_number.fraction));
		free(n);
		return NULL;
	}
	return n;
}

json_node* new_json_string_node(const dstring* string_value)
{
	json_node* n = malloc(sizeof(json_node));
	if(n == NULL)
		return NULL;
	n->type = JSON_STRING;
	if(!init_copy_dstring(&(n->json_string), string_value))
	{
		free(n);
		return NULL;
	}
	return n;
}

json_node* new_json_object_node(cy_uint element_count, const json_object_entry entries[])
{
	json_node* n = malloc(sizeof(json_node));
	if(n == NULL)
		return NULL;
	n->type = JSON_OBJECT;
	if(!initialize_hashmap(&(n->json_object), ROBINHOOD_HASHING, element_count + 4, &simple_hasher(hash_json_object_entry), &simple_comparator(((int (*)(const void*, const void*))compare_dstring)), offsetof(json_object_entry, embed_node)))
	{
		// if a hashmap for the object init fails, we free the node and quit
		free(n);
		return NULL;
	}
	for(cy_uint i = 0; i < element_count; i++)
	{
		if(!insert_in_json_object(n, &(entries[i].key), entries[i].value))
		{
			// if an insert in hashmap fails, we deinit the hashmap, free the node and quit
			deinitialize_hashmap(&(n->json_object));
			free(n);
			return NULL;
		}
	}
	return n;
}

json_node* new_json_array_node(cy_uint element_count, const json_node* elements[])
{
	json_node* n = malloc(sizeof(json_node));
	if(n == NULL)
		return NULL;
	n->type = JSON_ARRAY;
	if(!initialize_arraylist(&(n->json_array), element_count))
	{
		// if a arraylist init fails, we free the node and quit
		free(n);
		return NULL;
	}
	for(cy_uint i = 0; i < element_count; i++)
	{
		if(!append_to_json_array(n, elements[i]))
		{
			// if an append fails, we deinit the arraylist, free the node and quit
			deinitialize_arraylist(&(n->json_array));
			free(n);
			return NULL;
		}
	}
	return n;
}

int append_to_json_array(json_node* array_node_p, const json_node* node_p)
{
	if(array_node_p == NULL || array_node_p->type != JSON_ARRAY)
		return 0;

	if(is_full_arraylist(&(array_node_p->json_array)))
		expand_arraylist(&(array_node_p->json_array));

	return push_back_to_arraylist(&(array_node_p->json_array), node_p);
}

json_node* fetch_json_node_from_json_array(const json_node* array_node_p, cy_uint index)
{
	if(array_node_p == NULL || array_node_p->type != JSON_ARRAY || index >= get_element_count_arraylist(&(array_node_p->json_array)))
		return NULL;
	return (json_node*) get_from_front_of_arraylist(&(array_node_p->json_array), index);
}

int delete_from_json_array(json_node* array_node_p, cy_uint index)
{
	if(array_node_p == NULL || array_node_p->type != JSON_ARRAY)
		return 0;

	int removed = 0;
	json_node* n = NULL;
	if(index < get_element_count_arraylist(&(array_node_p->json_array)))
	{
		n = (json_node*) get_from_front_of_arraylist(&(array_node_p->json_array), index);
		removed = remove_elements_from_front_of_arraylist(&(array_node_p->json_array), index, 1);
	}

	if(removed)
		delete_json_node(n);

	return removed;
}

int insert_in_json_object(json_node* object_node_p, const dstring* key, const json_node* node_p)
{
	if(object_node_p == NULL || object_node_p->type != JSON_OBJECT)
		return 0;

	json_object_entry* e = malloc(sizeof(json_object_entry));
	// if the malloc call fails, we return a failure to insert the node
	if(e == NULL)
		return 0;

	// initialize the object entry
	if(!init_copy_dstring(&(e->key), key))
	{
		free(e);
		return 0;
	}
	e->value = (json_node*) node_p;
	initialize_rbhnode(&(e->embed_node));

	// expand if full
	if(get_element_count_hashmap(&(object_node_p->json_object)) == get_bucket_count_hashmap(&(object_node_p->json_object)))
		expand_hashmap(&(object_node_p->json_object), 2);

	// insert, and if it fails, we only need to, deinitialize the key and free the json_object_entry e
	int inserted = insert_in_hashmap(&(object_node_p->json_object), e);
	if(!inserted)
	{
		deinit_dstring(&(e->key));
		free(e);
		return 0;
	}

	return inserted;
}

json_node* fetch_json_node_from_json_object(const json_node* object_node_p, const dstring* key)
{
	if(object_node_p == NULL || object_node_p->type != JSON_OBJECT)
		return NULL;
	json_object_entry* e = (json_object_entry*) find_equals_in_hashmap(&(object_node_p->json_object), key);
	if(e == NULL)
		return NULL;
	return e->value;
}

int delete_from_json_object(json_node* object_node_p, const dstring* key)
{
	if(object_node_p == NULL || object_node_p->type != JSON_OBJECT)
		return 0;

	json_object_entry* e = (json_object_entry*) find_equals_in_hashmap(&(object_node_p->json_object), key);
	if(e == NULL) // if there is no such entry with the given key
		return 0;
	int removed = remove_from_hashmap(&(object_node_p->json_object), e);

	if(removed)
	{
		deinit_dstring(&(e->key));
		delete_json_node(e->value);
		free(e);
	}

	return removed;
}

static void notifier_on_remove_all_from_json_object_node(void* resource, const void* _joe)
{
	json_object_entry* entry_p = (json_object_entry*) _joe;
	deinit_dstring(&(entry_p->key));
	delete_json_node(entry_p->value);
	free(entry_p);
}

void delete_json_node(json_node* node_p)
{
	if(node_p == NULL || node_p == &true_json_node || node_p == &false_json_node)
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
			remove_all_from_hashmap(&(node_p->json_object), &((notifier_interface){NULL, notifier_on_remove_all_from_json_object_node}));
			deinitialize_hashmap(&(node_p->json_object));
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