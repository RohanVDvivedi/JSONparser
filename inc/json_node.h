#ifndef JSON_NODE_H
#define JSON_NODE_H

#include<stdio.h>
#include<stdlib.h>

#include<dstring.h>
#include<array.h>
#include<hashmap.h>

typedef enum json_data_type json_data_type;
enum json_data_type
{
	NULLE,		// dstring
	BOOLE,		// dstring
	NUMBER,		// dstring
	STRING,		// dstring
	ARRAY,		// array
	OBJECT,		// hashmap of dstring vs any other json_type

	ERROR		// this is when the json string has error and is not valid and can not be parsed
};

typedef struct json_node json_node;
struct json_node
{
	// type of node
	json_data_type type;

	// data for the node
	// NULLE			=> dstring (null)
	// BOOLE 			=> dstring (true, false)
	// NUMBER, STRING 	=> dstring ([-][0-9][.][0-9])
	// ARRAY 			=> array   (standard cutlery array of json_node*)
	// OBJECT 			=> hashmap (standard cutlery hashmap of dstring* vs json_node*)
	// ERROR 			=> NULL
	void* data_p;
};

// givens you a json_node with data_p as NULL 
json_node* get_new_json_node();

// initializes the given json_node
void initialize_json_node(json_node* jnode_p, json_data_type type, unsigned long long int expected_size);

// deletes the given json_node and all its children, and their children,,... recursively
void delete_json_node(json_node* jnode_p);

#endif