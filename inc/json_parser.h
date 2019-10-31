#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<json_node.h>
#include<state_stack.h>

#include<array.h>
#include<dstring.h>
#include<hashmap.h>

json_node* parse_json(dstring* json_string);

void delete_json(json_node* jnode);

#endif