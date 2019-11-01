#include<json_node.h>

unsigned long long int key_hashing_function(json_node* key)
{
	return ((dstring*)(key->data_p))->bytes_occupied;
}

int key_compare_function(json_node* key1, json_node* key2)
{
	return compare_dstring(((dstring*)(key1->data_p))->bytes_occupied, ((dstring*)(key2->data_p))->bytes_occupied);
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