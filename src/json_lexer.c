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

int get_next_lexeme_from_lexer(lexer* lxr, lexeme* lxm)
{
	// TODO
}