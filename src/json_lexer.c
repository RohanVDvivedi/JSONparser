#include<json_lexer.h>

int initialize_lexer(lexer* lxr, stream* byte_read_stream)
{
	// can not initialize lexeme, if the byte_read_stream is not readable
	if(!is_readable_stream(byte_read_stream))
		return 0;

	lxr->byte_read_stream = byte_read_stream;
	lxr->has_undone_lexeme = 0;
	return 1;
}

int undo_lexer(lexer* lxr, const lexeme* lxm)
{
	// this lexer can only hold, 1 undone lexeme
	if(lxr->has_undone_lexeme)
		return 0;

	lxr->undone_lexeme = (*lxm);
	lxr->has_undone_lexeme = 1;
	return 1;
}

void destroy_lexeme(lexeme* lxm)
{
	deinit_dstring(&(lxm->lexeme_str));
}

void deinitialize_lexer(lexer* lxr)
{
	// destroy any undone lexeme first
	if(lxr->has_undone_lexeme)
		destroy_lexeme(&(lxr->undone_lexeme));
	lxr->has_undone_lexeme = 0;
	lxr->byte_read_stream = NULL;
}

#define MAX_WHITESAPCES 2048

int get_next_lexeme_from_lexer(lexer* lxr, lexeme* lxm)
{
	int error = 0;

	// skip white spaces
	skip_whitespaces_from_stream(&(lxr->byte_read_stream), MAX_WHITESAPCES, &error);
	if(error)
		return 0;

	// read a byte
	char c;
	size_t byte_read = read_from_stream(&(lxr->byte_read_stream), &c, 1, &error);
	if(error)
		return 0;

	// end of stream token
	if(byte_read == 0)
	{
		lxm->lexeme_type = END_OF_STREAM;
		return 1;
	}

	// decode all single byte lexemes
	switch(c)
	{
		case '{' :
		{
			lxm->lexeme_type = CURLY_OPEN_BRACE;
			return 1;
		}
		case '{' :
		{
			lxm->lexeme_type = CLURLY_CLOSE_BRACE;
			return 1;
		}
		case '[' :
		{
			lxm->lexeme_type = SQUARE_OPEN_BRACE;
			return 1;
		}
		case ']' :
		{
			lxm->lexeme_type = SQUARE_CLOSE_BRACE;
			return 1;
		}
		case ',' :
		{
			lxm->lexeme_type = COMMA;
			return 1;
		}
		case ':' :
		{
			lxm->lexeme_type = COLON;
			return 1;
		}
	}

	// now you can decode fixed strings: true, false and null
	// TODO

	// decode STRING_LEXEME or NUMBER_LEXEME
	// TODO

	// if none return 0
}