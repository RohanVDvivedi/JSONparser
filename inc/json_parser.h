#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<stream.h>

#include<json_node.h>

#include<json_parser_error_codes.h>

// returns error
// if error is != JSON_NO_ERROR, then returned value is going to be NULL
// error values given in json_parser_error_codes.h
json_node* parse_json(stream* rs, size_t max_json_string_length, size_t max_json_number_length, int* error);

#endif