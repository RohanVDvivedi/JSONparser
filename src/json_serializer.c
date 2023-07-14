#include<json_serializer.h>

#include<stream_util.h>

static size_t write_json_string_to_stream(stream* ws, const dstring* str, int* error)
{
	const char* data = get_byte_array_dstring(str);
	cy_uint data_size = get_char_count_dstring(str);
	size_t bytes_written = 0;
	for(cy_uint i = 0; i < data_size; i++)
	{
		if(data[i] == '\n')
			bytes_written += write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\n"), error);
		else if(data[i] == '\t')
			bytes_written += write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\t"), error);
		else if(data[i] == '\v')
			bytes_written += write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\v"), error);
		else if(data[i] == '\r')
			bytes_written += write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\r"), error);
		else if(data[i] == '\f')
			bytes_written += write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\f"), error);
		else if(data[i] == '\b')
			bytes_written += write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\b"), error);
		else if(data[i] == '"')
			bytes_written += write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\\""), error);
		else if(data[i] == '\\')
			bytes_written += write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\\\"), error);
		else
			bytes_written += write_to_stream(ws, data + i, 1, error);
		if(*error)
			break;
	}
	return bytes_written;
}

int serialize_json(stream* ws, const json_node* node_p)
{
	int error = 0;

	if(node_p == NULL)
	{
		write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("null"), &error);
		if(error)
			return -1;
		return 0;
	}

	switch(node_p->type)
	{
		case JSON_BOOL:
		{
			if(node_p->json_bool)
				write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("true"), &error);
			else
				write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("false"), &error);
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
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\""), &error);
			if(error)
				return -1;
			write_json_string_to_stream(ws, &(node_p->json_string), &error);
			if(error)
				return -1;
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\""), &error);
			if(error)
				return -1;
			break;
		}
		case JSON_OBJECT:
		{
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("{"), &error);
			if(error)
				return -1;
			int is_first = 1;
			for(const json_object_entry* e = get_first_of_in_hashmap(&(node_p->json_object), ANY_IN_HASHMAP); e != NULL; e = get_next_of_in_hashmap(&(node_p->json_object), e, ANY_IN_HASHMAP))
			{
				if(!is_first)
				{
					write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring(","), &error);
					if(error)
						return -1;
				}
				write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\""), &error);
				if(error)
					return -1;
				write_json_string_to_stream(ws, &(e->key), &error);
				if(error)
					return -1;
				write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\":"), &error);
				if(error)
					return -1;
				if(serialize_json(ws, e->value))
					return -1;
				is_first = 0;
			}
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("}"), &error);
			if(error)
				return -1;
			break;
		}
		case JSON_ARRAY:
		{
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("["), &error);
			if(error)
				return -1;
			int is_first = 1;
			for(cy_uint i = 0; i < get_element_count_arraylist(&(node_p->json_array)); i++)
			{
				if(!is_first)
				{
					write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring(","), &error);
					if(error)
						return -1;
				}
				if(serialize_json(ws, get_nth_from_front_of_arraylist(&(node_p->json_array), i)))
					return -1;
				is_first = 0;
			}
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("]"), &error);
			if(error)
				return -1;
			break;
		}
	}
	return 0;
}