#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<dstring.h>

#include<json_node.h>
#include<state_stack.h>
#include<parserstate_jsontype.h>

json_node* parse_json(dstring* json_string);

#endif