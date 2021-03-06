#include<json_node.h>

#include<dstring.h>
#include<array.h>
#include<hashmap.h>

#include<stdio.h>
#include<stdlib.h>
#include<stddef.h>
#include<string.h>

// this the hashing fucntion used in the hashmap to hash 
// a json_node of string type in the json_node of type OBJECT
static unsigned int key_hashing_function(const void* entryp)
{
	return ((dstring*)(((object_entry*)entryp)->key->data_p))->bytes_occupied;
}

//to compare json_node keys in the hashmap of json_nodes in the json_node of type OBJECT
static int key_compare_function(const void* entry1, const void* entry2)
{
	return compare_dstring(((dstring*)(((object_entry*)entry1)->key->data_p)), ((dstring*)(((object_entry*)entry2)->key->data_p)));
}

json_node* get_new_json_node()
{
	json_node* jnode_p = (json_node*) malloc(sizeof(json_node));
	jnode_p->type = ERROR;
	jnode_p->data_p = NULL;
	return jnode_p;
}

object_entry* get_new_object_entry(json_node* key, json_node* value)
{
	object_entry* entryp = malloc(sizeof(object_entry));
	entryp->key = key;
	entryp->value = value;
	initialize_llnode(&(entryp->generic_hashmap_node));
	return entryp;
}

void initialize_json_node(json_node* jnode_p, json_data_type type, unsigned int expected_size)
{
	jnode_p->type = type;
	switch(jnode_p->type)
	{
		case ERROR:
		case NULLE:
		case BOOLE:
		{
			jnode_p->data_p = malloc(sizeof(dstring));
			init_dstring(jnode_p->data_p, NULL, 0);
			break;
		}
		case KEY:
		case NUMBER:
		case STRING:
		{
			jnode_p->data_p = malloc(sizeof(dstring));
			init_dstring(jnode_p->data_p, NULL, 0);
			break;
		}
		case ARRAY:
		{
			jnode_p->data_p = malloc(sizeof(array));
			initialize_array(jnode_p->data_p, expected_size);
			break;
		}
		case OBJECT:
		{
			jnode_p->data_p = malloc(sizeof(hashmap));
			initialize_hashmap(jnode_p->data_p, ELEMENTS_AS_LINKEDLIST, expected_size, key_hashing_function, key_compare_function, offsetof(object_entry, generic_hashmap_node));
			break;
		}
	}
}

void identify_dstring_json_node(json_node* jnode_p)
{
	if(jnode_p->type == ERROR)
	{
		if(compare_dstring(((dstring*)(jnode_p->data_p)), dstring_DUMMY_CSTRING("null")) == 0)
		{
			jnode_p->type = NULLE;
		}
		else if(compare_dstring(((dstring*)(jnode_p->data_p)), dstring_DUMMY_CSTRING("true")) == 0
		|| compare_dstring(((dstring*)(jnode_p->data_p)), dstring_DUMMY_CSTRING("false")) == 0)
		{
			jnode_p->type = BOOLE;
		}
		else
		{
			jnode_p->type = NUMBER;
			char* cstr = ((dstring*)(jnode_p->data_p))->cstring;
			unsigned int cstr_size = ((dstring*)(jnode_p->data_p))->bytes_occupied;
			int first_char = 1;
			int seen_decimal = 0;
			for(char* cstr_iter = cstr; cstr_iter < cstr + cstr_size;  cstr_iter++)
			{
				if(seen_decimal == 0 && *cstr_iter == '.')
				{
					seen_decimal = 1;
				}
				else if( (first_char == 0 || *cstr_iter != '-')
				 && (*cstr_iter < '0' || '9' < *cstr_iter) )
				{
					jnode_p->type = ERROR;
					break;
				}
				first_char = 0;
			}
		}
	}
}

static void delete_element(void* data_p, unsigned int index, const void* additional_params)
{
	delete_json_node(((json_node*)(data_p)));
}

static void delete_object_entry(const void* entryp, const void* additional_params)
{
	delete_json_node(((object_entry*)entryp)->key);
	delete_json_node(((object_entry*)entryp)->value);
	free(((object_entry*)entryp));
}


void delete_json_node(json_node* jnode_p)
{
	switch(jnode_p->type)
	{
		case ERROR:
		case NULLE:
		case BOOLE:
		case NUMBER:
		case KEY:
		case STRING:
		{
			deinit_dstring(jnode_p->data_p);
			free(jnode_p->data_p);
			break;
		}
		case ARRAY:
		{
			// delete the contents of the array, each of which is a json_node
			for_each_non_null_in_array(jnode_p->data_p, delete_element, NULL);

			// delete the array itself
			deinitialize_array(jnode_p->data_p);
			free(jnode_p->data_p);
			break;
		}
		case OBJECT:
		{
			// delete the keys and the corresponding json_node of the hashmap
			for_each_in_hashmap(((hashmap*)(jnode_p->data_p)), delete_object_entry, NULL);

			// delete the hash map itself
			deinitialize_hashmap(jnode_p->data_p);
			free(jnode_p->data_p);
			break;
		}
	}
	free(jnode_p);
}

static void sprint_json_node(dstring* append_str, const json_node* jnodep, unsigned int tabs);

static void sprint_object_entry(dstring* append_str, const void* entryp, unsigned int tabs)
{
	sprint_json_node(append_str, ((object_entry*)entryp)->key, tabs);
	sprint_json_node(append_str, ((object_entry*)entryp)->value, 0);
}

static void sprint_json_node(dstring* append_str, const json_node* jnodep, unsigned int tabs)
{
	if(jnodep == NULL)
	{
		snprintf_dstring(append_str, "NODE IS NULL\n");
		return;
	}
	else if(jnodep->type == ARRAY)
	{
		sprint_array(append_str, ((array*)(jnodep->data_p)), (void(*)(dstring*,const void*, unsigned int))sprint_json_node, tabs);
	}
	else if(jnodep->type == OBJECT)
	{
		sprint_hashmap(append_str, ((hashmap*)(jnodep->data_p)), sprint_object_entry, tabs);
	}
	else
	{
		sprint_chars(append_str, '\t', tabs);
		snprintf_dstring(append_str, "(%d)<", jnodep->type);
		concatenate_dstring(append_str, ((dstring*)(jnodep->data_p)));
		snprintf_dstring(append_str, ">");
	}
}

void print_json_node(const json_node* jnodep)
{
	dstring str;
	init_dstring(&str, NULL, 0);
	sprint_json_node(&str, jnodep, 0);
	printf_dstring(&str);
	deinit_dstring(&str);
}