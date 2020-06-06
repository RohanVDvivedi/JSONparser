#include<json_node.h>

// this the hashing fucntion used in the hashmap to hash 
// a json_node of string type in the json_node of type OBJECT
static unsigned long long int key_hashing_function(const void* entryp)
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
}

void initialize_json_node(json_node* jnode_p, json_data_type type, unsigned long long int expected_size)
{
	jnode_p->type = type;
	switch(jnode_p->type)
	{
		case ERROR:
		case NULLE:
		case BOOLE:
		{
			jnode_p->data_p = get_dstring("", 10);
		}
		case KEY:
		case NUMBER:
		case STRING:
		{
			jnode_p->data_p = get_dstring("", expected_size);
			break;
		}
		case ARRAY:
		{
			jnode_p->data_p = get_array(expected_size);
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
		static char* nulle       = "null";
		static char* boole_true  = "true";
		static char* boole_false = "false";
		if(strcmp(((dstring*)(jnode_p->data_p))->cstring, nulle) == 0)
		{
			jnode_p->type = NULLE;
		}
		else if(strcmp(((dstring*)(jnode_p->data_p))->cstring, boole_true) == 0
		|| strcmp(((dstring*)(jnode_p->data_p))->cstring, boole_false) == 0)
		{
			jnode_p->type = BOOLE;
		}
		else
		{
			jnode_p->type = NUMBER;
			char* cstr_iter = ((dstring*)(jnode_p->data_p))->cstring;
			int first_char = 1;
			int seen_decimal = 0;
			while(*cstr_iter != '\0')
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
				cstr_iter++;
			}
		}
	}
}

void delete_element(void* data_p, unsigned long long int index, const void* additional_params)
{
	if(data_p != NULL)
	{
		delete_json_node(((json_node*)(data_p)));
	}
}

static void delete_object_entry(const void* entryp, const void* additional_params)
{
	delete_json_node(((json_node*)(((object_entry*)entryp)->key)));
	delete_json_node(((json_node*)(((object_entry*)entryp)->value)));
	free((void*)entryp);
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
			delete_dstring(jnode_p->data_p);
			break;
		}
		case ARRAY:
		{
			// delete the contents of the array, each of which is a json_node
			for_each_in_array(jnode_p->data_p, delete_element, NULL);

			// delete the array itself
			delete_array(jnode_p->data_p);
			break;
		}
		case OBJECT:
		{
			// delete the keys and the corresponding json_node of the hashmap
			for_each_in_hashmap(jnode_p->data_p, delete_object_entry, NULL);

			// delete the hash map itself
			deinitialize_hashmap(jnode_p->data_p);

			free(jnode_p->data_p);
			break;
		}
	}
	free(jnode_p);
}

static void print_object_entry(const void* entryp)
{
	print_json_node(((object_entry*)entryp)->key);
	print_json_node(((object_entry*)entryp)->value);
}

void print_json_node(json_node* jnodep)
{
	if(jnodep == NULL)
	{
		printf("NODE IS NULL\n");
		return;
	}
	else if(jnodep->type == ARRAY)
	{
		print_array(((array*)(jnodep->data_p)), ((void (*)(const void* value))(print_json_node)));
	}
	else if(jnodep->type == OBJECT)
	{
		print_hashmap(((hashmap*)(jnodep->data_p)), print_object_entry);
	}
	else
	{
		printf("(%d)<%s>", jnodep->type, ((dstring*)(jnodep->data_p))->cstring);
	}
}