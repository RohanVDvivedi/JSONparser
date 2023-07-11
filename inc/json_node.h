#ifndef JSON_NODE_H
#define JSON_NODE_H

#include<linkedlist.h>
#include<array.h>
#include<hashmap.h>

#include<stdint.h>

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
		struct
		{
			cy_uint json_array_size;
			array json_array;
		};
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

json_node* new_json_bool_node(int bool_value);
json_node* new_json_unsigned_integer_node(uint64_t num_value);
json_node* new_json_integer_node(int64_t num_value);
json_node* new_json_float_node(long double num_value);
json_node* new_json_string_node(const dstring* string_value);
json_node* new_json_object_node(cy_uint element_count, const json_object_entry entries[]);
json_node* new_json_array_node(cy_uint element_count, const json_node* elements[]);

// append a node_p to array_node_p
int append_to_json_array(json_node* array_node_p, const json_node* node_p);

// remove an element at index from array_node_p and deletes it
int delete_from_jsom_array(json_node* array_node_p, cy_uint index);

// creates a new json_object_entry and inserts it into the json_object
int insert_in_json_object(json_node* object_node_p, const dstring* key, const json_node* node_p);

// removes a json_object_entry from object_node_p and deletes it
int delete_from_json_object(json_node* object_node_p, json_object_entry* entry_p);

// recursively deletes all of the json_nodes starting at node_p
void delete_json_node(json_node* node_p);

#endif