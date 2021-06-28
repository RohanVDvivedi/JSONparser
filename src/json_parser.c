#include<json_parser.h>

#include<json_lexer.h>

#include<stdio.h>

json_node* parse_json(const dstring* json_string)
{
	json_lexer jsonlxr;
	init_json_lexer(&jsonlxr, json_string);

	

	return NULL;
}