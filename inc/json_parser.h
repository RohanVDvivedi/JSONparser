#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<stream.h>

#include<json_node.h>

#define JSON_NO_ERROR          0
#define JSON_LEXER_ERROR      -1
#define JSON_PARSER_ERROR     -2
#define JSON_ERROR_IN_STREAM  -3
#define JSON_ALLOCATION_ERROR -4

// returns error if error is != 0, then returned value is going to be NULL
json_node* parse_json(stream* rs, size_t max_json_string_length, size_t max_json_number_length, int* error);

#endif