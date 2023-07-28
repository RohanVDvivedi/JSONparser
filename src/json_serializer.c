#include<json_serializer.h>

#include<stream_util.h>

static int serialize_escaping_json_string(stream* ws, const dstring* str)
{
	int stream_error = 0;
	const char* data = get_byte_array_dstring(str);
	cy_uint data_size = get_char_count_dstring(str);
	for(cy_uint i = 0; i < data_size; i++)
	{
		if(data[i] == '\n')
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\n"), &stream_error);
		else if(data[i] == '\t')
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\t"), &stream_error);
		else if(data[i] == '\v')
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\v"), &stream_error);
		else if(data[i] == '\r')
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\r"), &stream_error);
		else if(data[i] == '\f')
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\f"), &stream_error);
		else if(data[i] == '\b')
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\b"), &stream_error);
		else if(data[i] == '"')
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\\""), &stream_error);
		else if(data[i] == '\\')
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\\\\"), &stream_error);
		else
			write_to_stream(ws, data + i, 1, &stream_error);
		if(stream_error)
			return JSON_ERROR_IN_STREAM;
	}
	return JSON_NO_ERROR;
}

int serialize_json(stream* ws, const json_node* node_p)
{
	int stream_error = 0;

	if(node_p == NULL)
	{
		write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("null"), &stream_error);
		if(stream_error)
			return JSON_ERROR_IN_STREAM;
		return JSON_NO_ERROR;
	}

	switch(node_p->type)
	{
		case JSON_BOOL:
		{
			if(node_p->json_bool)
				write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("true"), &stream_error);
			else
				write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("false"), &stream_error);
			if(stream_error)
				return JSON_ERROR_IN_STREAM;
			break;
		}
		case JSON_NUM:
		{
			if(is_empty_dstring(&(node_p->json_number.exponent)))
				write_to_stream_formatted(ws, &stream_error, printf_dstring_format, printf_dstring_params(&(node_p->json_number.fraction)));
			else
				write_to_stream_formatted(ws, &stream_error, printf_dstring_format "e" printf_dstring_format, printf_dstring_params(&(node_p->json_number.fraction)), printf_dstring_params(&(node_p->json_number.exponent)));
			if(stream_error)
				return JSON_ERROR_IN_STREAM;
			break;
		}
		case JSON_STRING:
		{
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\""), &stream_error);
			if(stream_error)
				return JSON_ERROR_IN_STREAM;
			int error = serialize_escaping_json_string(ws, &(node_p->json_string));
			if(error)
				return error;
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\""), &stream_error);
			if(stream_error)
				return JSON_ERROR_IN_STREAM;
			break;
		}
		case JSON_OBJECT:
		{
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("{"), &stream_error);
			if(stream_error)
				return JSON_ERROR_IN_STREAM;
			int is_first = 1;
			for(const json_object_entry* e = get_first_of_in_hashmap(&(node_p->json_object), FIRST_OF_HASHMAP); e != NULL; e = get_next_of_in_hashmap(&(node_p->json_object), e, ANY_IN_HASHMAP))
			{
				if(!is_first)
				{
					write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring(","), &stream_error);
					if(stream_error)
						return JSON_ERROR_IN_STREAM;
				}
				write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\""), &stream_error);
				if(stream_error)
					return -1;
				int error = serialize_escaping_json_string(ws, &(e->key));
				if(error)
					return error;
				write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("\":"), &stream_error);
				if(stream_error)
					return JSON_ERROR_IN_STREAM;
				error = serialize_json(ws, e->value);
				if(error)
					return error;
				is_first = 0;
			}
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("}"), &stream_error);
			if(stream_error)
				return JSON_ERROR_IN_STREAM;
			break;
		}
		case JSON_ARRAY:
		{
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("["), &stream_error);
			if(stream_error)
				return JSON_ERROR_IN_STREAM;
			int is_first = 1;
			for(cy_uint i = 0; i < get_element_count_arraylist(&(node_p->json_array)); i++)
			{
				if(!is_first)
				{
					write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring(","), &stream_error);
					if(stream_error)
						return JSON_ERROR_IN_STREAM;
				}
				int error = serialize_json(ws, get_nth_from_front_of_arraylist(&(node_p->json_array), i));
				if(error)
					return error;
				is_first = 0;
			}
			write_dstring_to_stream(ws, &get_dstring_pointing_to_literal_cstring("]"), &stream_error);
			if(stream_error)
				return JSON_ERROR_IN_STREAM;
			break;
		}
	}
	return JSON_NO_ERROR;
}