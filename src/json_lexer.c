#include<json_lexer.h>

#include<ctype.h>

void init_json_lexer(json_lexer* json_lexer_p, dstring* json_string)
{
	json_lexer_p->next_token_start = json_string->cstring;
	json_lexer_p->json_string = json_string;
}

int get_next_json_lexeme(json_lexer* json_lexer_p, json_lexeme* json_lexeme_p)
{
	char* end_char_at = json_lexer_p->json_string->cstring + json_lexer_p->json_string->bytes_occupied;

	if(json_lexer_p->next_token_start == end_char_at)
	{
		json_lexeme_p->type = END_OF_JSON_STRING;
		return 1;
	}

	while((json_lexer_p->next_token_start != end_char_at) && isspace(*(json_lexer_p->next_token_start)))
		json_lexer_p->next_token_start++;

	if(json_lexer_p->next_token_start == end_char_at)
	{
		json_lexeme_p->type = END_OF_JSON_STRING;
		return 1;
	}

	switch(*(json_lexer_p->next_token_start))
	{
		case '\"':
		{
			json_lexeme_p->type = QUOTED_STRING;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start++;
			while((json_lexer_p->next_token_start != end_char_at) && (*(json_lexer_p->next_token_start++)) != '\"');
			if(json_lexer_p->next_token_start == end_char_at)
				return 0; //error
			json_lexeme_p->value.bytes_occupied = json_lexer_p->next_token_start - json_lexeme_p->value.cstring;
			return 1;
		}
		case '{':
		{
			json_lexeme_p->type = OPEN_CURLY_BRACKET;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start++;
			json_lexeme_p->value.bytes_occupied = 1;
			return 1;
		}
		case '}':
		{
			json_lexeme_p->type = CLOSE_CURLY_BRACKET;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start++;
			json_lexeme_p->value.bytes_occupied = 1;
			return 1;
		}
		case '[':
		{
			json_lexeme_p->type = OPEN_SQUARE_BRACKET;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start++;
			json_lexeme_p->value.bytes_occupied = 1;
			return 1;
		}
		case ']':
		{
			json_lexeme_p->type = CLOSE_SQUARE_BRACKET;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start++;
			json_lexeme_p->value.bytes_occupied = 1;
			return 1;
		}
		case ':':
		{
			json_lexeme_p->type = COLON;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start++;
			json_lexeme_p->value.bytes_occupied = 1;
			return 1;
		}
		case ',':
		{
			json_lexeme_p->type = COMMA;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start++;
			json_lexeme_p->value.bytes_occupied = 1;
			return 1;
		}
	}
}