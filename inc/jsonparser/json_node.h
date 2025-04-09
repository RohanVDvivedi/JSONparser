#ifndef JSON_NODE_H
#define JSON_NODE_H

#include<cutlery/linkedlist.h>
#include<cutlery/arraylist.h>
#include<cutlery/hashmap.h>

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
		arraylist json_array;
	};
};

typedef struct json_object_entry json_object_entry;
struct json_object_entry
{
	dstring key;
	json_node* value;

	// embedded node for the json_object
	rbhnode embed_node;
};

// recursively clones a json node
json_node* clone_json_node(const json_node* node_p);

json_node* new_json_bool_node(int bool_value);
json_node* new_json_unsigned_integer_node(uint64_t num_value);
json_node* new_json_integer_node(int64_t num_value);
json_node* new_json_float_node(long double num_value);
json_node* new_json_scientific_notation_node(long double fraction, int64_t exponent);
json_node* new_json_decimal_string_scientific_notation_node(const dstring* fraction, const dstring* exponent);
json_node* new_json_string_node(const dstring* string_value);
json_node* new_json_object_node(cy_uint element_count, const json_object_entry entries[]);
json_node* new_json_array_node(cy_uint element_count, const json_node* elements[]);

// append a node_p to array_node_p
int append_to_json_array(json_node* array_node_p, const json_node* node_p);

// fetch a sub json_node from json array
// returns NULL if array_node_p == NULL OR array_node_p.type != JSON_ARRAY OR index is out of range OR json_node at the index is NULL
json_node* fetch_json_node_from_json_array(const json_node* array_node_p, cy_uint index);

// remove an element at index from array_node_p and deletes it
int delete_from_json_array(json_node* array_node_p, cy_uint index);

// creates a new json_object_entry and inserts it into the json_object
int insert_in_json_object(json_node* object_node_p, const dstring* key, const json_node* node_p);

// fetch a sub json_node from json object
// returns NULL if object_node_p == NULL OR object_node_p->type != JSON_OBJECT OR json object entry with given key does not exist in object_node_p OR json_node at the given key is NULL
json_node* fetch_json_node_from_json_object(const json_node* object_node_p, const dstring* key);

// removes a json_object_entry from object_node_p corresponding to key and deletes it
int delete_from_json_object(json_node* object_node_p, const dstring* key);

// recursively deletes all of the json_nodes starting at node_p
void delete_json_node(json_node* node_p);

#define fetch_json_node_from(js, X) _Generic((X),\
											const dstring*:     fetch_json_node_from_json_object,\
											dstring*:           fetch_json_node_from_json_object,\
											unsigned int:       fetch_json_node_from_json_array,\
											int:                fetch_json_node_from_json_array,\
											const unsigned int: fetch_json_node_from_json_array,\
											const int:          fetch_json_node_from_json_array,\
											const cy_uint:      fetch_json_node_from_json_array,\
											cy_uint:            fetch_json_node_from_json_array)\
											(js, X)

#include<jsonparser/json_accessor.h>

#endif