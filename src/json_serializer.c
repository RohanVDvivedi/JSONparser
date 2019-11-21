#include<json_serializer.h>

int serialize_json(dstring* result, json_node* node_p)
{
	switch(node_p->type)
	{
		case NULLE :
		case BOOLE :
		case NUMBER :
		{
			concatenate_dstring(result, ((dstring*)(node_p->data_p)));
			break;
		}
		case KEY :
		case STRING :
		{
			append_to_dstring(result, "\"");
			concatenate_dstring(result, ((dstring*)(node_p->data_p)));
			append_to_dstring(result, "\"");
			break;
		}
		case ARRAY :
		{
			break;
		}
		case OBJECT :
		{
			break;
		}
		default :
		{
			return 1;
		}
	}
	return 0;
}