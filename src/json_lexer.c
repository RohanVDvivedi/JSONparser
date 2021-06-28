#include<json_lexer.h>

#include<stdio.h>
#include<ctype.h>

void init_json_lexer(json_lexer* json_lexer_p, const dstring* json_string)
{
	json_lexer_p->next_token_start = json_string->cstring;
	json_lexer_p->json_string = json_string;
}

int get_next_json_lexeme(json_lexer* json_lexer_p, json_lexeme* json_lexeme_p)
{
	char* end_char_at = json_lexer_p->json_string->cstring + json_lexer_p->json_string->bytes_occupied;

	if(json_lexer_p->next_token_start == NULL || json_lexer_p->next_token_start == end_char_at)
	{
		json_lexeme_p->type = END_OF_JSON_STRING;
		json_lexeme_p->value = ((dstring){});
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
			static dstring find_string = {.cstring="null", .bytes_occupied = 4, .bytes_allocated = 0};
			json_lexeme_p->type = NULL_STRING;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start;
			unsigned int seen = 0;
			int is_find_string = 1;
			while((json_lexer_p->next_token_start != end_char_at) && (seen < find_string.bytes_occupied))
			{
				if((*(json_lexer_p->next_token_start++)) != json_lexeme_p->value.cstring[seen++])
				{
					is_find_string = 0;
					break;
				}
			}
			json_lexeme_p->value.bytes_occupied = json_lexer_p->next_token_start - json_lexeme_p->value.cstring;
			if(is_find_string && seen == find_string.bytes_occupied)
				return 1;
			else
				return 0;
		}
		case 't':
		{
			static dstring find_string = {.cstring="true", .bytes_occupied = 4, .bytes_allocated = 0};
			json_lexeme_p->type = TRUE_STRING;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start;
			unsigned int seen = 0;
			int is_find_string = 1;
			while((json_lexer_p->next_token_start != end_char_at) && (seen < find_string.bytes_occupied))
			{
				if((*(json_lexer_p->next_token_start++)) != json_lexeme_p->value.cstring[seen++])
				{
					is_find_string = 0;
					break;
				}
			}
			json_lexeme_p->value.bytes_occupied = json_lexer_p->next_token_start - json_lexeme_p->value.cstring;
			if(is_find_string && seen == find_string.bytes_occupied)
				return 1;
			else
				return 0;
		}
		case 'f':
		{
			static dstring find_string = {.cstring="false", .bytes_occupied = 5, .bytes_allocated = 0};
			json_lexeme_p->type = FALSE_STRING;
			json_lexeme_p->value.cstring = json_lexer_p->next_token_start;
			unsigned int seen = 0;
			int is_find_string = 1;
			while((json_lexer_p->next_token_start != end_char_at) && (seen < find_string.bytes_occupied))
			{
				if((*(json_lexer_p->next_token_start++)) != json_lexeme_p->value.cstring[seen++])
				{
					is_find_string = 0;
					break;
				}
			}
			json_lexeme_p->value.bytes_occupied = json_lexer_p->next_token_start - json_lexeme_p->value.cstring;
			if(is_find_string && seen == find_string.bytes_occupied)
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
			else if(isspace(*(json_lexer_p->next_token_start)))
			{
				json_lexeme_p->type = WHITESPACES_STRING;
				json_lexeme_p->value.cstring = json_lexer_p->next_token_start++;
				json_lexeme_p->value.bytes_occupied = 1;
				while((json_lexer_p->next_token_start != end_char_at) && isspace(*(json_lexer_p->next_token_start)))
					json_lexer_p->next_token_start++;
				json_lexeme_p->value.bytes_occupied = json_lexer_p->next_token_start - json_lexeme_p->value.cstring;
				return 1;
			}
			else
				return 0;
		}
	}
}