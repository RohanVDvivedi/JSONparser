#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<stream.h>
#include<json_node.h>

// returns NULL on error
json_node* parse_json(stream* rs);

#endif