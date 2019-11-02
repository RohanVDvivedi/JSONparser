#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<json_node.h>
#include<state_stack.h>

#include<dstring.h>

json_node* parse_json(dstring* json_string);

void print_json_node(json_node* jnodep);

#endif