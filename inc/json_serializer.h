#ifndef JSON_SERIALIZER
#define JSON_SERIALIZER

#include<stream.h>

#include<json_node.h>

#include<json_parser_error_codes.h>

// returns int value, suggesting error, from error codes given in json_parser_error_codes.h
// return value of JSON_NO_ERROR is a success
int serialize_json(stream* ws, const json_node* node_p);

#endif