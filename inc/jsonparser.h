#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include<stdio.h>
#include<stdlib.h>

#include<jsonnode.h>
#include<array.h>
#include<dstring.h>
#include<hashmap.h>
#include<stack.h>

json_node* parse_json(dstring* json_string);

#endif