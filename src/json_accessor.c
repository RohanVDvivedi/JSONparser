#include<jsonparser/json_accessor.h>

json_node* get_json_node_from_json_node(json_node* node_p, json_accessor acs)
{
	while(node_p != NULL && acs.keys_length > 0)
	{
		json_key k = acs.keys_list[0];

		if(k.is_array_index && node_p->type == JSON_ARRAY)
			node_p = fetch_json_node_from_json_array(node_p, k.index);
		else if((!k.is_array_index) && node_p->type == JSON_ARRAY)
			node_p = fetch_json_node_from_json_object(node_p, &(k.key));
		else
			return NULL;

		acs.keys_list++;
		acs.keys_length--;
	}

	return node_p;
}