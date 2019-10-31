#ifndef JSON_NODE_H
#define JSON_NODE_H

typedef enum json_data_type json_data_type;
enum json_data_type
{
	BOOLE,		// boolean as a uint8_t
	NUMBER,		// dstring
	STRING,		// dstring
	ARRAY,		// array
	OBJECT,		// hashmap of dstring vs any other json_type

	ERROR		// this is when the json string has error and is not valid and can not be parsed
};

typedef struct json_node json_node;
struct json_node
{
	json_data_type type;
	void* data_p;
};

#endif