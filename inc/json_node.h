#ifndef JSON_NODE_H
#define JSON_NODE_H

#include<hashmap.h>
#include<array.h>
#include<dstring.h>

typedef enum json_data_type json_data_type;
enum json_data_type
{
	BOOLEAN,	// int  (true = 1, false = 0)
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

	// BOOLEAN			=> int
	// NUMBER, STRING 	=> dstring
	// ARRAY 			=> array
	// OBJECT 			=> hashmap
	
	union
	{
		int boolean;
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