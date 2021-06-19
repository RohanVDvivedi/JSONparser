#ifndef JSON_NODE_H
#define JSON_NODE_H

#include<stdbool.h>
#include<dstring.h>
#include<array.h>
#include<hashmap.h>

typedef enum json_data_type json_data_type;
enum json_data_type
{
	BOOLEAN,	// bool as in stdbool.h
	NUMBER,		// dstring  ([-][0-9]*[.][0-9]*)
	STRING,		// dstring  (*)
	
	ARRAY,		// array   (a dynamic array of json_node*, a null value represents a null of json)
	OBJECT		// hashmap of object_attribute (hashmap of unique dstring vs json_node*, a null value represents a null of json)
};

typedef struct json_node json_node;
struct json_node
{
	// type of node
	json_data_type type;

	// BOOLEAN			=> bool
	// NUMBER, STRING 	=> dstring
	// ARRAY 			=> array
	// OBJECT 			=> hashmap
	
	union
	{
		bool boolean;
		dstring value;
		array array;
		hashmap object;
	};
};

typedef struct object_attribute object_attribute;
struct object_attribute
{
	dstring key;
	json_node* value;
};

#endif