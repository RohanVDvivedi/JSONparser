#ifndef JSON_NODE_H
#define JSON_NODE_H

#include<linkedlist.h>
#include<array.h>
#include<hashmap.h>

typedef enum json_node_type json_node_type;
enum json_node_type
{
	JSON_BOOL,
	JSON_NUM,
	JSON_STRING,
	JSON_OBJECT,
	JSON_ARRAY
};

typedef struct json_num json_num;
struct json_num
{
	dstring integer;
	dstring fraction;
	dstring exponent;
};

typedef struct json_node json_node;
struct json_node
{
	json_node_type type;

	union
	{
		int json_bool : 1;
		json_num json_number;
		dstring json_string;
		hashmap json_object;
		array json_array;
	};
};

typedef struct json_object_entry json_object_entry;
struct json_object_entry
{
	dstring key;
	json_node* value;

	// to be used for linking in the json_object
	llnode embed_node;
};

json_node* new_json_bool_node(int bool);
json_node* new_json_unsigned_integer_node(uint64_t num);
json_node* new_json_integer_node(int64_t num);
json_node* new_json_float_node(long double num);
json_node* new_json_string_node(const dstring* json_string);
json_node* new_json_object_node(cy_uint element_count, const json_object_entry entries[]);
json_node* new_json_array_node(cy_uint element_count, const json_node* elements[]);

// creates a new json_object_entry and inserts it into the json_object
int insert_in_json_object(json_node* object_node_p, const dstring* key, const json_node* node_p);

// removes a json_object_entry and deletes it
int remove_from_json_object(json_node* object_node_p, json_object_entry* entry_p);

void delete_json_node(json_node* node_p);

#endif