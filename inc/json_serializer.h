#ifndef JSON_SERIALIZER
#define JSON_SERIALIZER

#include<dstring.h>
#include<json_node.h>

void serialize_json(dstring* result, const json_node* node_p);

#endif