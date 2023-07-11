#include<json_serializer.h>

#include<stream_util.h>

int serialize_json(stream* ws, const json_node* node_p)
{
	int error = 0;

	if(node_p == NULL)
	{
		write_to_stream_formatted(ws, &error, "null");
		if(error)
			return -1;
		return 0;
	}

	switch(node_p->type)
	{
		case JSON_BOOL:
		{
			write_to_stream_formatted(ws, &error, "%s", (node_p->json_bool ? "true" : "false"));
			if(error)
				return -1;
			break;
		}
		case JSON_NUM:
		{
			if(is_empty_dstring(&(node_p->json_number.exponent)))
				write_to_stream_formatted(ws, &error, printf_dstring_format, printf_dstring_params(&(node_p->json_number.fraction)));
			else
				write_to_stream_formatted(ws, &error, printf_dstring_format "e" printf_dstring_format, printf_dstring_params(&(node_p->json_number.fraction)), printf_dstring_params(&(node_p->json_number.exponent)));
			if(error)
				return -1;
			break;
		}
		case JSON_STRING:
		{
			write_to_stream_formatted(ws, &error, "\""printf_dstring_format"\"", printf_dstring_params(&(node_p->json_string)));
			if(error)
				return -1;
			break;
		}
		case JSON_OBJECT:
		{
			write_to_stream_formatted(ws, &error, "{");
			if(error)
				return -1;
			int is_first = 1;
			for(const json_object_entry* e = get_first_of_in_hashmap(&(node_p->json_object), ANY_IN_HASHMAP); e != NULL; e = get_next_of_in_hashmap(&(node_p->json_object), e, ANY_IN_HASHMAP))
			{
				if(!is_first)
					write_to_stream_formatted(ws, &error, ",");
				write_to_stream_formatted(ws, &error, "\""printf_dstring_format"\":", printf_dstring_params(&(e->key)));
				if(error)
					return -1;
				if(serialize_json(ws, e->value))
					return -1;
				is_first = 0;
			}
			write_to_stream_formatted(ws, &error, "}");
			if(error)
				return -1;
			break;
		}
		case JSON_ARRAY:
		{
			write_to_stream_formatted(ws, &error, "[");
			if(error)
				return -1;
			int is_first = 1;
			for(cy_uint i = 0; i < get_element_count_arraylist(&(node_p->json_array)); i++)
			{
				if(!is_first)
					write_to_stream_formatted(ws, &error, ",");
				if(serialize_json(ws, get_nth_from_front_of_arraylist(&(node_p->json_array), i)))
					return -1;
				is_first = 0;
			}
			write_to_stream_formatted(ws, &error, "]");
			if(error)
				return -1;
			break;
		}
	}
	return 0;
}