#ifndef JSON_NODE_H
#define JSON_NODE_H

#include<stdbool.h>
#include<dstring.h>
#include<array.h>
#include<hashmap.h>

typedef enum json_data_type json_data_type;
enum json_data_type
{
	JSON_BOOLEAN,	// bool as in stdbool.h
	JSON_NUMBER,		// dstring  ([-][0-9]*[.][0-9]*)
	JSON_STRING,		// dstring  (*)
	
	JSON_ARRAY,		// array   (a dynamic array of json_node*, a null value represents a null of json)
	JSON_OBJECT		// hashmap of object_attribute (hashmap of unique dstring vs json_node*, a null value represents a null of json)
};

typedef struct json_node json_node;
struct json_node
{
	// type of node
	json_data_type type;

	// JSON_BOOLEAN				=> bool
	// JSON_NUMBER, JSON_STRING => dstring
	// JSON_ARRAY 				=> array
	// JSON_OBJECT 				=> hashmap
	
	union
	{
		bool boolean;
		dstring value;
		struct
		{
			unsigned int array_length;
			array array;
		};
		hashmap object;
	};
};

typedef struct object_attribute object_attribute;
struct object_attribute
{
	dstring key;
	json_node* value;
};

json_node* get_new_boolean_json_node(bool value);
int set_boolean_json_node(json_node* node_p, bool value);

json_node* get_new_string_json_node(const dstring* value);
int set_string_json_node(json_node* node_p, const dstring* value);

json_node* get_new_integer_json_node(long long int value);
int set_integer_json_node(json_node* node_p, long long int value);

json_node* get_new_double_json_node(double value);
int set_double_json_node(json_node* node_p, double value);

json_node* get_new_array_json_node();
int init_array_json_node(json_node* node_p);
int insert_in_json_array(json_node* array_node_p, unsigned int index, json_node* node_p);
int append_to_json_array(json_node* array_node_p, json_node* node_p);
json_node* get_from_json_array(const json_node* array_node_p, unsigned int index);
int remove_all_from_json_array(json_node* array_node_p);

json_node* get_new_object_json_node();
int init_object_json_node(json_node* node_p);
int insert_in_json_object(json_node* object_node_p, const dstring* key, json_node* node_p);
int exists_in_json_object(const json_node* object_node_p, const dstring* key);
json_node* get_from_json_object(const json_node* object_node_p, const dstring* key);
int remove_all_from_json_object(json_node* object_node_p);

void delete_json_tree(json_node* node_p);

#endif