#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<cutlery/stream.h>

#include<jsonparser/json_node.h>

#include<jsonparser/json_parser_error_codes.h>

// returns error
// if error is != JSON_NO_ERROR, then returned value is going to be NULL
// error values given in json_parser_error_codes.h
json_node* parse_json(stream* rs, cy_uint max_json_string_length, cy_uint max_json_number_length, int* error);

#endif