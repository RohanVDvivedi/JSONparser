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
		case 'n':
		{
			static char* null_string = "null";
			unsigned int null_string_length = 4;
			json_lexeme_p->type = NULL_STRING;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start;
			json_lexeme_p->value.bytes_occupied = null_string_length;
			unsigned int seen = 0;
			int is_null_string = 1;
			while((json_lexer_p->next_token_start != end_char_at) && (seen < null_string_length))
			{
				if((*(json_lexer_p->next_token_start++)) != null_string[seen++])
				{
					is_null_string = 0;
					break;
				}
			}
			if(is_null_string && seen == null_string_length)
				return 1;
			else
				return 0;
		}
		case 't':
		{
			static char* true_string = "true";
			unsigned int true_string_length = 4;
			json_lexeme_p->type = TRUE_STRING;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start;
			json_lexeme_p->value.bytes_occupied = true_string_length;
			unsigned int seen = 0;
			int is_true_string = 1;
			while((json_lexer_p->next_token_start != end_char_at) && (seen < true_string_length))
			{
				if((*(json_lexer_p->next_token_start++)) != true_string[seen++])
				{
					is_true_string = 0;
					break;
				}
			}
			if(is_true_string && seen == true_string_length)
				return 1;
			else
				return 0;
		}
		case 'f':
		{
			static char* false_string = "false";
			unsigned int false_string_length = 5;
			json_lexeme_p->type = FALSE_STRING;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start;
			json_lexeme_p->value.bytes_occupied = false_string_length;
			unsigned int seen = 0;
			int is_false_string = 1;
			while((json_lexer_p->next_token_start != end_char_at) && (seen < false_string_length))
			{
				if((*(json_lexer_p->next_token_start++)) != false_string[seen++])
				{
					is_false_string = 0;
					break;
				}
			}
			if(is_false_string && seen == false_string_length)
				return 1;
			else
				return 0;
		}
		default :
		{
			if((*(json_lexer_p->next_token_start)) == '-' || ('0' <= (*(json_lexer_p->next_token_start)) && (*(json_lexer_p->next_token_start)) <= '9'))
			{
				json_lexeme_p->type = NUMBER_STRING;
				json_lexeme_p->value.cstring = json_lexer_p->next_token_start;

				if((*(json_lexer_p->next_token_start)) == '-')
				{
					json_lexer_p->next_token_start++;
					if(json_lexer_p->next_token_start == end_char_at)
						return 0;
				}

				while( (json_lexer_p->next_token_start != end_char_at) && ('0' <= (*(json_lexer_p->next_token_start)) && (*(json_lexer_p->next_token_start)) <= '9') )
					json_lexer_p->next_token_start++;

				if( (json_lexer_p->next_token_start != end_char_at) && (*(json_lexer_p->next_token_start)) == '.')
				{
					json_lexer_p->next_token_start++;
					if(json_lexer_p->next_token_start == end_char_at)
						return 0;

					while( (json_lexer_p->next_token_start != end_char_at) && ('0' <= (*(json_lexer_p->next_token_start)) && (*(json_lexer_p->next_token_start)) <= '9') )
						json_lexer_p->next_token_start++;
				}

				json_lexeme_p->value.bytes_occupied = json_lexer_p->next_token_start - json_lexeme_p->value.cstring;
				return 1;
			}
			else
				return 0;
		}
	}
}