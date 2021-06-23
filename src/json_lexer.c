#include<json_lexer.h>

void init_json_lexer(json_lexer* json_lexer_p, dstring* json_string)
{
	json_lexer_p->next_token_start = json_string->cstring;
	json_lexer_p->json_string = json_string;
}

int get_next_json_lexeme(json_lexer* json_lexer_p, json_lexeme* lexeme_p);