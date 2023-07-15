#include<json_lexer.h>

#include<stream_util.h>

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
	skip_whitespaces_from_stream(lxr->byte_read_stream, MAX_WHITESAPCES, &error);
	if(error)
		return 0;

	// read a byte
	char c;
	size_t byte_read = read_from_stream(lxr->byte_read_stream, &c, 1, &error);
	if(error)
		return 0;

	// end of stream token
	if(byte_read == 0)
	{
		lxm->type = END_OF_STREAM;
		return 1;
	}

	// all white spaces must have been skipped, on excess white spaces we quit
	if(is_whitespace_char(c))
		return 0;

	// decode all single byte lexemes
	switch(c)
	{
		case '{' :
		{
			lxm->type = CURLY_OPEN_BRACE;
			return 1;
		}
		case '}' :
		{
			lxm->type = CLURLY_CLOSE_BRACE;
			return 1;
		}
		case '[' :
		{
			lxm->type = SQUARE_OPEN_BRACE;
			return 1;
		}
		case ']' :
		{
			lxm->type = SQUARE_CLOSE_BRACE;
			return 1;
		}
		case ',' :
		{
			lxm->type = COMMA;
			return 1;
		}
		case ':' :
		{
			lxm->type = COLON;
			return 1;
		}
	}

	// unread the single byte that was read
	unread_from_stream(lxr->byte_read_stream, &c, 1);

	// now you can decode fixed strings: true, false and null
	dstring string_to_skip = get_dstring_pointing_to_literal_cstring("true");
	size_t bytes_skipped = skip_dstring_from_stream(lxr->byte_read_stream, &string_to_skip, &error);
	if(error)
		return 0;
	if(bytes_skipped > 0)
	{
		lxm->type = TRUE;
		return 1;
	}

	string_to_skip = get_dstring_pointing_to_literal_cstring("false");
	bytes_skipped = skip_dstring_from_stream(lxr->byte_read_stream, &string_to_skip, &error);
	if(error)
		return 0;
	if(bytes_skipped > 0)
	{
		lxm->type = FALSE;
		return 1;
	}

	string_to_skip = get_dstring_pointing_to_literal_cstring("null");
	bytes_skipped = skip_dstring_from_stream(lxr->byte_read_stream, &string_to_skip, &error);
	if(error)
		return 0;
	if(bytes_skipped > 0)
	{
		lxm->type = NULL_LEXEME;
		return 1;
	}

	// decode STRING_LEXEME or NUMBER_LEXEME
	// TODO

	// if none return 0
}