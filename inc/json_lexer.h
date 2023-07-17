#ifndef JSON_LEXER_H
#define JSON_LEXER_H

#include<stream.h>
#include<dstring.h>

// types of JSON lexemes
typedef enum lexeme_type lexeme_type;
enum lexeme_type
{
	// generated when there are no characters to be read from stream
	END_OF_STREAM = 0,

	CURLY_OPEN_BRACE,
	CURLY_CLOSE_BRACE,

	SQUARE_OPEN_BRACE,
	SQUARE_CLOSE_BRACE,

	COMMA,
	COLON,

	TRUE_LEXEME,
	FALSE_LEXEME,

	NULL_LEXEME,

	// only for the below 2 lexeme types is the lexeme_str valid
	STRING_LEXEME,
	NUMBER_LEXEME
};

typedef struct lexeme lexeme;
struct lexeme
{
	// type of lexeme
	lexeme_type type;

	// string contents of lexeme
	// this attribute of the lexeme is valid only for STRING_LEXEME and NUMBER_LEXEME
	dstring lexeme_str;
};

/*
* lexer can only hold 1 undo lexeme
*/

typedef struct lexer lexer;
struct lexer
{
	// lexer can hold one undo lexeme
	unsigned int has_undone_lexeme : 1;

	// lexeme that as undone
	lexeme undone_lexeme;

	// maximum json string length lexeme to be read
	size_t max_json_string_length;

	// maximum json number lexeme_str length to be read
	size_t max_json_number_length;

	// stream to produce lexemes from
	stream* byte_read_stream;
};

// all the below 4 functions return 1 for success and 0 for error

int initialize_lexer(lexer* lxr, stream* byte_read_stream, size_t max_json_string_length, size_t max_json_number_length);

int undo_lexer(lexer* lxr, lexeme* lxm);

void destroy_lexeme(lexeme* lxm);

void deinitialize_lexer(lexer* lxr);

#include<json_parser.h>

// return, same error codes as json_parser.h, 0 (NO_ERROR) implies success
// the only error codes this function returns are JSON_LEXER_ERROR and JSON_ERROR_IN_STREAM
// this function must not be called again after it returns error
int get_next_lexeme_from_lexer(lexer* lxr, lexeme* lxm);

#endif