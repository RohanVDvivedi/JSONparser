#ifndef JSON_LEXER_H
#define JSON_LEXER_H

typedef enum json_lexeme_type json_lexeme_type;
enum json_lexeme_type
{
	QUOTED_STRING,			// "string"
	NUMBER_STRING,			// number
	TRUE_STRING,			// true
	FALSE_STRING,			// false
	NULL_STRING,			// null
	OPEN_CURLY_BRACKET,		// {
	CLOSE_CURLY_BRACKET,	// }
	OPEN_SQUARE_BRACKET,	// [
	CLOSE_SQUARE_BRACKET,	// ]
	COLON,					// :
	COMMA,					// ,
	END_OF_JSON_STRING
};

typedef struct json_lexeme json_lexeme;
struct json_lexeme
{
	json_lexeme_type type;	// type of lexeme

	// the value points to the position of the lexeme in the json_string
	dstring value;
};

typedef struct json_lexer json_lexer;
struct json_lexer
{
	char* next_token_start;

	dstring* json_dstring;
};

void init_json_lexer(json_lexer* json_lexer_p, dstring* json_string);

// after this function call the lex points to the next lexeme after its previous value
// returns 1 for success, a return value of 0 represents lexical error
int get_next_json_lexeme(json_lexer* json_lexer_p, json_lexeme* lexeme_p);

#endif