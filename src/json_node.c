#include<json_node.h>

json_node* new_json_bool_node(int bool);
json_node* new_json_unsigned_integer_node(uint64_t num);
json_node* new_json_integer_node(int64_t num);
json_node* new_json_float_node(long double num);
json_node* new_json_string_node(const dstring* json_string);
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

void delete_json_node(json_node* node_p);