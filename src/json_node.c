#include<json_node.h>

json_node* get_new_json_node(json_data_type type)
{
	json_node* jnode_p = (json_node*) malloc(sizeof(json_node));
	jnode_p->type = type;
	switch(jnode_p->type)
	{
		case BOOLE:
		{
			break;
		}
		case NUMBER:
		case STRING:
		{
			break;
		}
		case ARRAY:
		{
			break;
		}
		case OBJECT:
		{
			break;
		}
		default:
		{
			jnode_p->data_p = NULL;
			break;
		}
	}
	return jnode_p;
}

void delete_json_node(json_node* jnode_p)
{
	switch(jnode_p->type)
	{
		case BOOLE:
		{
			break;
		}
		case NUMBER:
		case STRING:
		{
			break;
		}
		case ARRAY:
		{
			break;
		}
		case OBJECT:
		{
			break;
		}
		default:
		{
			break;
		}
	}
	free(jnode_p);
}