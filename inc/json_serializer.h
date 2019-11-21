#ifndef JSON_SERIALIZER
#define JSON_SERIALIZER

#include<dstring.h>
#include<array.h>
#include<hashmap.h>

#include<json_node.h>

// returns -1 if an ERROR is encountered in the json node, else 0 is returned
int serialize_json(dstring* result, const json_node* node_p);

#endif