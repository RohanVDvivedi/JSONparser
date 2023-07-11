#include<json_serializer.h>

#include<stream_util.h>

int serialize_json(stream* ws, const json_node* node_p)
{
	int error = 0;

	if(node_p == NULL)
	{
		write_to_stream_formatted(ws, &error, "%s", "null");
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
			write_to_stream_formatted(ws, &error, printf_dstring_format, printf_dstring_params(&(node_p->json_string)));
			if(error)
				return -1;
			break;
		}
		case JSON_OBJECT:
		{
			break;
		}
		case JSON_ARRAY:
		{
			break;
		}
	}
	return 0;
}