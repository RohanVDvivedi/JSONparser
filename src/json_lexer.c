#include<json_lexer.h>

#include<stream_util.h>

int initialize_lexer(lexer* lxr, stream* byte_read_stream, size_t max_json_string_length, size_t max_json_number_length)
{
	// can not initialize lexeme, if the byte_read_stream is not readable OR if the maximum json_string or json_number length os 0
	if((!is_readable_stream(byte_read_stream)) || (max_json_string_length == 0) || (max_json_number_length == 0))
		return 0;

	lxr->max_json_string_length = max_json_string_length;
	lxr->max_json_number_length = max_json_number_length + 4; // extra 4 for +/- sign for fracton and exponent, decimal point and last one for 'e' and 'E'
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

static int get_next_string_lexeme(lexer* lxr, lexeme* lxm)
{
	// make dstring_empty before proceeding
	make_dstring_empty(&(lxm->lexeme_str));

	int error = 0;
	char c;

	dstring bytes_read;
	init_empty_dstring(&bytes_read, 0);

	// read first quotation
	size_t byte_read = read_from_stream(lxr->byte_read_stream, &c, 1, &error);
	if(error || byte_read == 0)
		goto FAILURE;
	concatenate_char(&bytes_read, c);

	if(c != '"')
		goto FAILURE;

	while(1)
	{
		byte_read = read_from_stream(lxr->byte_read_stream, &c, 1, &error);
		if(error || byte_read == 0)
			goto FAILURE;
		concatenate_char(&bytes_read, c);

		// if last, then exit
		if(c == '"')
		{
			lxm->type = STRING_LEXEME;
			deinit_dstring(&bytes_read);
			return 1;
		}

		// if the json_string lexeme size is as big as the max_json_string_length permitted, 
		// then we are not supposed to append any further and we quit with a failure
		if(get_char_count_dstring(&(lxm->lexeme_str)) >= lxr->max_json_string_length)
			goto FAILURE;

		if(c == '\\')
		{
			byte_read = read_from_stream(lxr->byte_read_stream, &c, 1, &error);
			if(error || byte_read == 0)
				goto FAILURE;
			concatenate_char(&bytes_read, c);

			switch(c)
			{
				case 'n' :
				{
					concatenate_char(&(lxm->lexeme_str), '\n');
					break;
				}
				case 'r' :
				{
					concatenate_char(&(lxm->lexeme_str), '\r');
					break;
				}
				case 'v' :
				{
					concatenate_char(&(lxm->lexeme_str), '\v');
					break;
				}
				case 'f' :
				{
					concatenate_char(&(lxm->lexeme_str), '\f');
					break;
				}
				case 't' :
				{
					concatenate_char(&(lxm->lexeme_str), '\t');
					break;
				}
				case 'b' :
				{
					concatenate_char(&(lxm->lexeme_str), '\b');
					break;
				}
				case '"' :
				{
					concatenate_char(&(lxm->lexeme_str), '"');
					break;
				}
				case '\\' :
				{
					concatenate_char(&(lxm->lexeme_str), '\\');
					break;
				}
			}
		}
		else
			concatenate_char(&(lxm->lexeme_str), c);
	}

	FAILURE :
	unread_dstring_from_stream(lxr->byte_read_stream, &bytes_read);
	deinit_dstring(&bytes_read);
	return 0;
}

static int get_next_number_lexeme(lexer* lxr, lexeme* lxm)
{
	int error = 0;
	char c;
	size_t byte_read;
	make_dstring_empty(&(lxm->lexeme_str));

	// read a byte
	byte_read = read_from_stream(lxr->byte_read_stream, &c, 1, &error);
	if(error || byte_read == 0)
		goto FAILURE;

	// if digit, unread it
	if(is_digit_char(c))
		unread_from_stream(lxr->byte_read_stream, &c, 1);
	else if(c == '+' || c == '-') // its sign of the fraction
	{
		// can not read a character, if we will cross the maximum limit
		if(get_char_count_dstring(&(lxm->lexeme_str)) >= lxr->max_json_number_length)
		{
			unread_from_stream(lxr->byte_read_stream, &c, 1);
			goto FAILURE;
		}
		concatenate_char(&(lxm->lexeme_str), c);
	}
	else // fail if none of the above
		goto FAILURE;

	// only one decimal point can be read, and we track that
	int is_decimal_point_read = 0;
	unsigned int bytes_numeric_count = 0;

	while(1)
	{
		// read a byte
		byte_read = read_from_stream(lxr->byte_read_stream, &c, 1, &error);
		if(error || (byte_read == 0 && bytes_numeric_count == 0))
			goto FAILURE;
		else if(byte_read == 0)
			goto SUCCESS;

		if(is_digit_char(c))
		{
			// can not read a character, if we will cross the maximum limit
			if(get_char_count_dstring(&(lxm->lexeme_str)) >= lxr->max_json_number_length)
			{
				unread_from_stream(lxr->byte_read_stream, &c, 1);
				goto FAILURE;
			}
			concatenate_char(&(lxm->lexeme_str), c);
			bytes_numeric_count++;
		}
		else if(c == '.')
		{
			if(is_decimal_point_read)
			{
				unread_from_stream(lxr->byte_read_stream, &c, 1);
				goto FAILURE;
			}
			else
			{
				is_decimal_point_read = 1;
				// can not read a character, if we will cross the maximum limit
				if(get_char_count_dstring(&(lxm->lexeme_str)) >= lxr->max_json_number_length)
				{
					unread_from_stream(lxr->byte_read_stream, &c, 1);
					goto FAILURE;
				}
				concatenate_char(&(lxm->lexeme_str), c);
				bytes_numeric_count = 0;
			}
		}
		else if(c == 'e' || c == 'E')
		{
			// can not read a character, if we will cross the maximum limit
			if(get_char_count_dstring(&(lxm->lexeme_str)) >= lxr->max_json_number_length)
			{
				unread_from_stream(lxr->byte_read_stream, &c, 1);
				goto FAILURE;
			}
			concatenate_char(&(lxm->lexeme_str), c);
			bytes_numeric_count = 0;
			break;
		}
		else
		{
			if(bytes_numeric_count == 0)
			{
				unread_from_stream(lxr->byte_read_stream, &c, 1);
				goto FAILURE;
			}
			else
				goto SUCCESS;
		}
	}

	// read a byte
	byte_read = read_from_stream(lxr->byte_read_stream, &c, 1, &error);
	if(error || byte_read == 0)
		goto FAILURE;

	// if digit, unread it
	if(is_digit_char(c))
		unread_from_stream(lxr->byte_read_stream, &c, 1);
	else if(c == '+' || c == '-') // its sign of the fraction
	{
		// can not read a character, if we will cross the maximum limit
		if(get_char_count_dstring(&(lxm->lexeme_str)) >= lxr->max_json_number_length)
		{
			unread_from_stream(lxr->byte_read_stream, &c, 1);
			goto FAILURE;
		}
		concatenate_char(&(lxm->lexeme_str), c);
	}
	else // fail if none of the above
		goto FAILURE;

	bytes_numeric_count = 0;

	while(1)
	{
		// read a byte
		byte_read = read_from_stream(lxr->byte_read_stream, &c, 1, &error);
		if(error || (byte_read == 0 && bytes_numeric_count == 0))
			goto FAILURE;
		else if(byte_read == 0)
			goto SUCCESS;

		if(is_digit_char(c))
		{
			// can not read a character, if we will cross the maximum limit
			if(get_char_count_dstring(&(lxm->lexeme_str)) >= lxr->max_json_number_length)
			{
				unread_from_stream(lxr->byte_read_stream, &c, 1);
				goto FAILURE;
			}
			concatenate_char(&(lxm->lexeme_str), c);
			bytes_numeric_count++;
		}
		else
		{
			if(bytes_numeric_count == 0)
			{
				unread_from_stream(lxr->byte_read_stream, &c, 1);
				goto FAILURE;
			}
			else
				goto SUCCESS;
		}
	}

	SUCCESS :
	return 1;

	FAILURE :
	unread_dstring_from_stream(lxr->byte_read_stream, &(lxm->lexeme_str));
	deinit_dstring(&(lxm->lexeme_str));
	return 0;
}

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

	// it can be true false or null, but none of then have '"' character as their first character
	if(c != '"')
	{
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
	}

	// decode STRING_LEXEME or NUMBER_LEXEME

	// read next first byte and decide if it is a string
	if(c == '"')
	{
		// return true if lexeme was read successfully
		if(get_next_string_lexeme(lxr, lxm))
			return 1;
	}
	else // if the first character is not a '"' then this could be a NUMBER_LEXEME
	{
		// check if it is a NUMBER_LEXEME
		if(get_next_number_lexeme(lxr, lxm))
			return 1;
	}

	// if none
	return 0;
}