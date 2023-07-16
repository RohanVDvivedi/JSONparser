#include<json_parser.h>

#include<json_lexer.h>

#include<stdio.h>

static json_node* parse_json_array_node(lexer* lxr);
static json_node* parse_json_object_node(lexer* lxr);

static json_node* parse_json_node(lexer* lxr)
{
	json_node* js = NULL;

	lexeme lxm;

	// read the next lexeme to ch
	if(!get_next_lexeme_from_lexer(lxr, &lxm))
		goto FAIL;

	switch(lxm.type)
	{
		case NULL_LEXEME :
		{
			destroy_lexeme(&lxm);
			goto EXIT;
		}
		case TRUE :
		{
			js = new_json_bool_node(1);
			destroy_lexeme(&lxm);
			goto EXIT;
		}
		case FALSE :
		{
			js = new_json_bool_node(0);
			destroy_lexeme(&lxm);
			goto EXIT;
		}
		case STRING_LEXEME :
		{
			js = new_json_string_node(&(lxm.lexeme_str));
			destroy_lexeme(&lxm);
			goto EXIT;
		}
		case NUMBER_LEXEME :
		{
			const dstring* e = get_dstring_pointing_to_literal_cstring("e");
			const dstring* E = get_dstring_pointing_to_literal_cstring("E");

			const dstring* split_by = NULL;

			if(contains_dstring_RK(&(lxm.lexeme_str), e) != INVALID_INDEX)
				split_by = e;
			else if(contains_dstring_RK(&(lxm.lexeme_str), E) != INVALID_INDEX)
				split_by = E;

			// both the below dstrings are suppossed to be point_dstr
			dstring fraction;
			dstring exponent;

			if(split_by == NULL)
			{
				fraction = get_dstring_pointing_to_dstring(&(lxm.lexeme_str));
				exponent = get_dstring_pointing_to_literal_cstring("");
			}
			else
				exponent = split_dstring(&(lxm.lexeme_str), split_by, &fraction);

			js = new_json_decimal_string_scientific_notation_node(&fraction, &exponent);

			destroy_lexeme(&lxm);
			goto EXIT;
		}
		case CURLY_OPEN_BRACE :
		{
			undo_lexer(lxr, &lxm);
			js = parse_json_object_node(lxr);
			goto EXIT;
		}
		case SQUARE_OPEN_BRACE :
		{
			undo_lexer(lxr, &lxm);
			js = parse_json_array_node(lxr);
			goto EXIT;
		}
		default :
		{
			destroy_lexeme(&lxm);
			goto FAIL;
		}
	}

	FAIL:;
	delete_json_node(js);
	js = NULL;
	EXIT:;
	return js;
}

static json_node* parse_json_array_node(lexer* lxr)
{
	json_node* js = NULL;

	lexeme lxm;

	// read the starting of the json array, a '['
	if(!get_next_lexeme_from_lexer(lxr, &lxm))
		goto FAIL;
	destroy_lexeme(&lxm);
	if(lxm.type != SQUARE_OPEN_BRACE)
		goto FAIL;

	// initialize a new json_object node
	js = new_json_array_node(0, NULL);

	// empty object case if next lexeme is a ']'
	if(!get_next_lexeme_from_lexer(lxr, &lxm))
		goto FAIL;
	if(lxm.type == SQUARE_CLOSE_BRACE)
	{
		destroy_lexeme(&lxm);
		goto EXIT;
	}
	else
		undo_lexer(lxr, &lxm);

	while(1)
	{
		// parse value
		json_node* value = parse_json_node(lxr);

		// insert parsed json key value into the json object
		append_to_json_array(js, value);

		// after key comes a ',' or a ']'
		if(!get_next_lexeme_from_lexer(lxr, &lxm))
			goto FAIL;
		destroy_lexeme(&lxm);
		if(lxm.type == COMMA)
			continue;
		else if(lxm.type == SQUARE_CLOSE_BRACE)
			goto EXIT;
		else
			goto FAIL;
	}

	FAIL:;
	delete_json_node(js);
	js = NULL;

	EXIT:;
	return js;
}

static json_node* parse_json_object_node(lexer* lxr)
{
	json_node* js = NULL;

	lexeme lxm;

	// read the starting of the json object, a '{'
	if(!get_next_lexeme_from_lexer(lxr, &lxm))
		goto FAIL;
	destroy_lexeme(&lxm);
	if(lxm.type != CURLY_OPEN_BRACE)
		goto FAIL;

	// initialize a new json_object node
	js = new_json_object_node(0, NULL);

	// empty object case if next lexeme is a '}'
	if(!get_next_lexeme_from_lexer(lxr, &lxm))
		goto FAIL;
	if(lxm.type == CURLY_CLOSE_BRACE)
	{
		destroy_lexeme(&lxm);
		goto EXIT;
	}
	else
		undo_lexer(lxr, &lxm);

	while(1)
	{
		// read a string key
		if(!get_next_lexeme_from_lexer(lxr, &lxm))
			goto FAIL;
		if(lxm.type != STRING_LEXEME)
		{
			destroy_lexeme(&lxm);
			goto FAIL;
		}
		dstring key;
		init_copy_dstring(&key, &(lxm.lexeme_str));
		destroy_lexeme(&lxm);

		// after key comes a ':'
		if(!get_next_lexeme_from_lexer(lxr, &lxm))
			goto FAIL;
		destroy_lexeme(&lxm);
		if(lxm.type != COLON)
			goto FAIL;

		// parse value
		json_node* value = parse_json_node(lxr);

		// insert parsed json key value into the json object
		insert_in_json_object(js, &key, value);

		// after key comes a ',' or a '}'
		if(!get_next_lexeme_from_lexer(lxr, &lxm))
			goto FAIL;
		destroy_lexeme(&lxm);
		if(lxm.type == COMMA)
			continue;
		else if(lxm.type == CURLY_CLOSE_BRACE)
			goto EXIT;
		else
			goto FAIL;
	}

	FAIL:;
	delete_json_node(js);
	js = NULL;

	EXIT:;
	return js;
}

json_node* parse_json(stream* rs, size_t max_json_string_length, size_t max_json_number_length)
{
	lexer lxr;
	initialize_lexer(&lxr, rs, max_json_string_length, max_json_number_length);

	lexeme lxm;

	json_node* js = NULL;

	if(!get_next_lexeme_from_lexer(&lxr, &lxm))
		goto EXIT;

	if(lxm.type == CURLY_OPEN_BRACE)
	{
		undo_lexer(&lxr, &lxm);
		js = parse_json_object_node(&lxr);
	}
	else if(lxm.type == SQUARE_OPEN_BRACE)
	{
		undo_lexer(&lxr, &lxm);
		js = parse_json_array_node(&lxr);
	}
	else
	{
		destroy_lexeme(&lxm);
		goto EXIT;
	}


	EXIT:;
	deinitialize_lexer(&lxr);
	return js;
}