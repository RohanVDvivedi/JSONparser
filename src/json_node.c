#include<json_node.h>

unsigned long long int key_hashing_function(json_node* key)
{
	return ((dstring*)(key->data_p))->bytes_occupied;
}

int key_compare_function(json_node* key1, json_node* key2)
{
	return compare_dstring(((dstring*)(key1->data_p)), ((dstring*)(key2->data_p)));
}

json_node* get_new_json_node()
{
	json_node* jnode_p = (json_node*) malloc(sizeof(json_node));
	jnode_p->type = ERROR;
	jnode_p->data_p = NULL;
	return jnode_p;
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
			jnode_p->data_p = get_hashmap(expected_size, (unsigned long long int(*)(const void*))key_hashing_function, (int(*)(const void*, const void*))key_compare_function, ELEMENTS_AS_RED_BLACK_BST);
			break;
		}
		default:
		{
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

void delete_json_node(json_node* jnode_p)
{
	switch(jnode_p->type)
	{
		case NULLE:
		case BOOLE:
		case NUMBER:
		case STRING:
		{
			delete_dstring(jnode_p->data_p);
			break;
		}
		case ARRAY:
		{
			// delete the contents of the array, each of which is a json_node

			// delete the array itself
			delete_array(jnode_p->data_p);
			break;
		}
		case OBJECT:
		{
			// delete the keys and the corresponding json_node of the hashmap

			// delete the hash map itself
			delete_hashmap(jnode_p->data_p);
			break;
		}
		default:
		{
			break;
		}
	}
	free(jnode_p);
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
		print_hashmap(((hashmap*)(jnodep->data_p)), ((void (*)(const void* value))(print_json_node)), ((void (*)(const void* value))(print_json_node)));
	}
	else
	{
		printf("(%d)<%s>", jnodep->type, ((dstring*)(jnodep->data_p))->cstring);
	}
}