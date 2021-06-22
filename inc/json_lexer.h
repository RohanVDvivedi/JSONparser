#ifndef JSON_LEXER_H
#define JSON_LEXER_H

typedef enum json_lexeme_type json_lexeme_type;
enum json_lexeme_type
{
	START_OF_JSON = 0,
	QUOTED_STRING,
	NUMBER_STRING,
	TRUE_STRING,
	FALSE_STRING,
	NULL_STRING,
	OPEN_CURLY_BRACKET,
	CLOSE_CURLY_BRACKET,
	OPEN_SQUARE_BRACKET,
	CLOSE_SQUARE_BRACKET,
	COLON,
	COMMA,
	END_OF_JSON
};

typedef struct json_lexeme json_lexeme;
struct json_lexeme
{
	json_lexeme_type type;	// type of lexeme

	// the value points to the position of the lexeme in the json_string
	dstring value;
};

#define INIT_JSON_LEXEME ((json_lexeme){.type = START_OF_JSON, .value = {}})

// after this function call the lex points to the next lexeme after its previous value
// returns 1 for success, a return value of 0 represents lexical error
int lexer_get_next_lexeme(json_lexeme* lex, const dstring* json_string);

#endif