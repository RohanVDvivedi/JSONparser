#include<json_serializer.h>

#include<stdio.h>

typedef struct state_dstring state_dstring;
struct state_dstring
{
	union
	{
		unsigned int cur_iteration;
		unsigned int tot_iteration;
	};
	dstring* result;
};

static void serialize_json_wrapper_array(json_node* node_p, unsigned int index, state_dstring* state_result)
{
	// not the first entry, hence the comma 
	if(index >= state_result->tot_iteration)
		return;

	// not the first entry, hence the comma
	if(index != 0)
		snprintf_dstring(state_result->result, ",");

	serialize_json(state_result->result, node_p);
}

static void serialize_json_wrapper_hashmap(const object_attribute* attr, state_dstring* state_result)
{
	// not the first entry, hence the comma 
	if(state_result->cur_iteration != 0)
		snprintf_dstring(state_result->result, ",");

	// serialize the entry
	snprintf_dstring(state_result->result, "\"%.*s\"", attr->key.bytes_occupied, attr->key.cstring);
	snprintf_dstring(state_result->result, ":");
	serialize_json(state_result->result, attr->value);

	state_result->cur_iteration++;
}

void serialize_json(dstring* result, const json_node* node_p)
{
	if(node_p == NULL)
	{
		snprintf_dstring(result, "null");
		return;
	}
	switch(node_p->type)
	{
		case JSON_BOOLEAN :
		{
			snprintf_dstring(result, "%s", node_p->boolean ? "true" : "false");
			break;
		}
		case JSON_NUMBER :
		{
			concatenate_dstring(result, &(node_p->value));
			break;
		}
		case JSON_STRING :
		{
			snprintf_dstring(result, "\"%.*s\"", node_p->value.bytes_occupied, node_p->value.cstring);
			break;
		}
		case JSON_ARRAY :
		{
			snprintf_dstring(result, "[");
			state_dstring state_result = {.tot_iteration = 0, .result = result};
			for_each_in_array(&(node_p->array), (void (*)(void*, unsigned int, const void*))serialize_json_wrapper_array, &state_result);
			snprintf_dstring(result, "]");
			break;
		}
		case JSON_OBJECT :
		{
			snprintf_dstring(result, "{");
			state_dstring state_result = {.cur_iteration = 0, .result = result};
			for_each_in_hashmap(&(node_p->object), (void (*)(const void*, const void*))serialize_json_wrapper_hashmap, &state_result);
			snprintf_dstring(result, "}");
			break;
		}
	}
}