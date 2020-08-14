#ifndef JSON_NODE_H
#define JSON_NODE_H

#include<linkedlist.h>

#include<json_data_type.h>

typedef struct json_node json_node;
struct json_node
{
	// type of node
	json_data_type type;

	// data for the node
	// NULLE			=> dstring (null)
	// BOOLE 			=> dstring (true, false)
	// NUMBER		 	=> dstring ([-][0-9][.][0-9])
	// STRING 			=> dstring (*)
	// ARRAY 			=> array   (standard cutlery array of json_node*)
	// OBJECT 			=> hashmap (standard cutlery hashmap of json_node*(STRING mandatorily) vs json_node*)
	
	// ERROR 			=> dstring (*), used when the json data is unidentified
	void* data_p;
};

typedef struct object_entry object_entry;
struct object_entry
{
	json_node* key;
	json_node* value;

	// used for resolving collisions
	llnode generic_hashmap_node;
};

// givens you a json_node with data_p as NULL 
json_node* get_new_json_node();

// returns a newly created json node for given key and value
object_entry* get_new_object_entry(json_node* key, json_node* value);

// initializes the given json_node
void initialize_json_node(json_node* jnode_p, json_data_type type, unsigned int expected_size);

// used to convert ERROR -> NULLE, BOOLE, NUMBER
void identify_dstring_json_node(json_node* jnode_p);

// deletes the given json_node and all its children, and their children,,... recursively
void delete_json_node(json_node* jnode_p);

void print_json_node(json_node* jnodep);

#endif