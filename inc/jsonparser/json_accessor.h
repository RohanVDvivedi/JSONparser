#ifndef JSON_ACCESSOR_H
#define JSON_ACCESSOR_H

typedef struct json_key json_key;
struct json_key
{
	int is_array_index; // set if keying into a JSON_ARRAY node
	union
	{
		dstring key; // for JSON_OBJECT
		cy_uint index; // for JSON_ARRAY
	};
};

#define JSON_OBJECT_KEY(key_v)         ((json_key){.is_array_index = 0, .key = get_dstring_pointing_to_dstring(key_v)})
#define JSON_OBJECT_KEY_literal(key_p) ((json_key){.is_array_index = 0, .key = get_dstring_pointing_to_literal_cstring(key_p)})

#define JSON_ARRAY_INDEX(index_v)      ((json_key){.is_array_index = 1, .index = index_v})

typedef struct json_accessor json_accessor;
struct json_accessor
{
	uint64_t keys_length;

	json_key* keys_list;
};

#define STATIC_JSON_ACCESSOR(...) ((json_accessor){ .keys_length = sizeof((json_key []){ __VA_ARGS__ })/sizeof(json_key), .keys_list = (json_key []){ __VA_ARGS__ } })

#endif