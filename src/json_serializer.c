#include<json_serializer.h>

typedef struct state_dstring state_dstring;
struct state_dstring
{
	unsigned long long int state;
	dstring* result;
};

static void serialize_json_wrapper_array(json_node* node_p, unsigned long long int index, state_dstring* state_result)
{
	if(state_result->state != -1 && node_p != NULL)
	{
		if(index != 0)
		{
			append_to_dstring(state_result->result, ",");
		}
		state_result->state = serialize_json(state_result->result, node_p);
	}
}

static void serialize_json_wrapper_hashmap(const object_entry* entryp, state_dstring* state_result)
{
	const json_node* key_node_p = entryp->key;
	const json_node* value_node_p = entryp->value;
	if(state_result->state != -1)
	{
		if(state_result->state != 0)
		{
			append_to_dstring(state_result->result, ",");
		}
		int res1 = 0;
		res1 = serialize_json(state_result->result, key_node_p);
		append_to_dstring(state_result->result, ":");
		if(res1 != -1)
		{
			res1 = serialize_json(state_result->result, value_node_p);
		}
		if(res1 != -1)
		{
			state_result->state++;
		}
	}
}

int serialize_json(dstring* result, const json_node* node_p)
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
			append_to_dstring(result, "[");
			state_dstring state_result = {.state = 0, .result = result};
			for_each_in_array(((array*)(node_p->data_p)), (void (*)(void*, unsigned long long int, const void*))serialize_json_wrapper_array, &state_result);
			append_to_dstring(result, "]");
			if(state_result.state != 0)
			{
				return -1;
			}
			break;
		}
		case OBJECT :
		{
			append_to_dstring(result, "{");
			state_dstring state_result = {.state = 0, .result = result};
			for_each_in_hashmap(((hashmap*)(node_p->data_p)), (void (*)(const void*, const void*))serialize_json_wrapper_hashmap, &state_result);
			append_to_dstring(result, "}");
			if(state_result.state != 0)
			{
				return -1;
			}
			break;
		}
		default :
		{
			return -1;
		}
	}
	return 0;
}