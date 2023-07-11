#ifndef JSON_SERIALIZER
#define JSON_SERIALIZER

#include<stream.h>

#include<json_node.h>

// returns -1 if an error is encountered in serializing the json node, else 0 is returned
int serialize_json(stream* ws, const json_node* node_p);

#endif