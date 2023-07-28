#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<stream.h>

#include<json_node.h>

#include<json_parser_error_codes.h>

// returns error if error is != 0, then returned value is going to be NULL
json_node* parse_json(stream* rs, size_t max_json_string_length, size_t max_json_number_length, int* error);

#endif