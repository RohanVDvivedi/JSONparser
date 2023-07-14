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

	// all of \n, \t, \v or ' ' charcters
	WHITESPACE,

	CURLY_OPEN_BRACE,
	CLURLY_CLOSE_BRACE,

	SQUARE_OPEN_BRACE,
	SQUARE_CLOSE_BRACE,

	COMMA,
	COLON,

	TRUE,
	FALSE,

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

	// stream to produce lexemes from
	stream* byte_read_stream;
};

// all the below functions return 1 for success and 0 for error

int initialize_lexer(lexer* lxr, stream* byte_read_stream);

int get_next_lexeme_from_lexer(lexer* lxr, lexeme* lxm);

int undo_lexer(lexer* lxr, const lexeme* lxm);

void destroy_lexeme(lexeme* lxm);

void deinitialize_lexer(lexer* lxr);

#endif