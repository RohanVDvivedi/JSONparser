#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<stream.h>

#include<json_node.h>

#define NO_ERROR           0
#define LEXER_ERROR        1
#define PARSE_ERROR        2
#define STREAM_ERROR       3
#define ALLOCATION_ERROR   4

// returns NULL on error if error is != 0
json_node* parse_json(stream* rs, size_t max_json_string_length, size_t max_json_number_length);

#endif