#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<dstring.h>
#include<json_node.h>

json_node* parse_json(const dstring* json_string);

#endif