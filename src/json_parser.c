#include<json_parser.h>

#include<json_lexer.h>

static json_node* parse_json_array_node(lexer* lxr, int* error);
static json_node* parse_json_object_node(lexer* lxr, int* error);

static json_node* parse_json_node(lexer* lxr, int* error)
{
	json_node* js = NULL;

	(*error) = JSON_NO_ERROR;
	lexeme lxm;

	// read the next lexeme to ch
	if(((*error) = get_next_lexeme_from_lexer(lxr, &lxm)))
		goto FAIL;

	switch(lxm.type)
	{
		case NULL_LEXEME :
		{
			destroy_lexeme(&lxm);
			goto EXIT;
		}
		case TRUE_LEXEME :
		{
			js = new_json_bool_node(1);
			if(js == NULL)
				(*error) = JSON_ALLOCATION_ERROR;
			destroy_lexeme(&lxm);
			goto EXIT;
		}
		case FALSE_LEXEME :
		{
			js = new_json_bool_node(0);
			if(js == NULL)
				(*error) = JSON_ALLOCATION_ERROR;
			destroy_lexeme(&lxm);
			goto EXIT;
		}
		case STRING_LEXEME :
		{
			js = new_json_string_node(&(lxm.lexeme_str));
			if(js == NULL)
				(*error) = JSON_ALLOCATION_ERROR;
			destroy_lexeme(&lxm);
			goto EXIT;
		}
		case NUMBER_LEXEME :
		{
			const dstring* e = &get_dstring_pointing_to_literal_cstring("e");
			const dstring* E = &get_dstring_pointing_to_literal_cstring("E");

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

			if(js == NULL)
				(*error) = JSON_ALLOCATION_ERROR;

			destroy_lexeme(&lxm);
			goto EXIT;
		}
		case CURLY_OPEN_BRACE :
		{
			undo_lexer(lxr, &lxm);
			js = parse_json_object_node(lxr, error);
			goto EXIT;
		}
		case SQUARE_OPEN_BRACE :
		{
			undo_lexer(lxr, &lxm);
			js = parse_json_array_node(lxr, error);
			goto EXIT;
		}
		default :
		{
			destroy_lexeme(&lxm);
			(*error) = JSON_PARSER_ERROR;
			goto FAIL;
		}
	}

	FAIL:;
	delete_json_node(js);
	js = NULL;
	EXIT:;
	return js;
}

static json_node* parse_json_array_node(lexer* lxr, int* error)
{
	json_node* js = NULL;

	(*error) = JSON_NO_ERROR;
	lexeme lxm;

	// read the starting of the json array, a '['
	if(((*error) = get_next_lexeme_from_lexer(lxr, &lxm)))
		goto FAIL;
	destroy_lexeme(&lxm);
	if(lxm.type != SQUARE_OPEN_BRACE)
	{
		(*error) = JSON_PARSER_ERROR;
		goto FAIL;
	}

	// initialize a new json_object node
	js = new_json_array_node(0, NULL);
	if(js == NULL)
	{
		(*error) = JSON_ALLOCATION_ERROR;
		goto FAIL;
	}

	// empty object case if next lexeme is a ']'
	if(((*error) = get_next_lexeme_from_lexer(lxr, &lxm)))
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
		json_node* value = parse_json_node(lxr, error);
		if((*error))
			goto FAIL;

		// insert parsed json key value into the json object
		if(!append_to_json_array(js, value))
		{
			delete_json_node(value);
			(*error) = JSON_ALLOCATION_ERROR;
			goto FAIL;
		}

		// after key comes a ',' or a ']'
		if(((*error) = get_next_lexeme_from_lexer(lxr, &lxm)))
			goto FAIL;
		destroy_lexeme(&lxm);
		if(lxm.type == COMMA)
			continue;
		else if(lxm.type == SQUARE_CLOSE_BRACE)
			goto EXIT;
		else
		{
			(*error) = JSON_PARSER_ERROR;
			goto FAIL;
		}
	}

	FAIL:;
	delete_json_node(js);
	js = NULL;

	EXIT:;
	return js;
}

static json_node* parse_json_object_node(lexer* lxr, int* error)
{
	json_node* js = NULL;

	(*error) = JSON_NO_ERROR;
	lexeme lxm;

	// read the starting of the json object, a '{'
	if(((*error) = get_next_lexeme_from_lexer(lxr, &lxm)))
		goto FAIL;
	destroy_lexeme(&lxm);
	if(lxm.type != CURLY_OPEN_BRACE)
	{
		(*error) = JSON_PARSER_ERROR;
		goto FAIL;
	}

	// initialize a new json_object node
	js = new_json_object_node(0, NULL);
	if(js == NULL)
	{
		(*error) = JSON_ALLOCATION_ERROR;
		goto FAIL;
	}

	// empty object case if next lexeme is a '}'
	if(((*error) = get_next_lexeme_from_lexer(lxr, &lxm)))
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
		if(((*error) = get_next_lexeme_from_lexer(lxr, &lxm)))
			goto FAIL;
		if(lxm.type != STRING_LEXEME)
		{
			destroy_lexeme(&lxm);
			(*error) = JSON_PARSER_ERROR;
			goto FAIL;
		}
		dstring key;
		if(!init_copy_dstring(&key, &(lxm.lexeme_str)))
		{
			destroy_lexeme(&lxm);
			(*error) = JSON_ALLOCATION_ERROR;
			goto FAIL;
		}
		destroy_lexeme(&lxm);

		// after key comes a ':'
		if(((*error) = get_next_lexeme_from_lexer(lxr, &lxm)))
			goto FAIL;
		destroy_lexeme(&lxm);
		if(lxm.type != COLON)
		{
			(*error) = JSON_PARSER_ERROR;
			goto FAIL;
		}

		// parse value
		json_node* value = parse_json_node(lxr, error);
		if((*error))
		{
			deinit_dstring(&key);
			goto FAIL;
		}

		// insert parsed json key value into the json object
		if(!insert_in_json_object(js, &key, value))
		{
			deinit_dstring(&key);
			delete_json_node(value);
			goto FAIL;
		}

		deinit_dstring(&key);

		// after key comes a ',' or a '}'
		if(((*error) = get_next_lexeme_from_lexer(lxr, &lxm)))
			goto FAIL;
		destroy_lexeme(&lxm);
		if(lxm.type == COMMA)
			continue;
		else if(lxm.type == CURLY_CLOSE_BRACE)
			goto EXIT;
		else
		{
			(*error) = JSON_PARSER_ERROR;
			goto FAIL;
		}
	}

	FAIL:;
	delete_json_node(js);
	js = NULL;

	EXIT:;
	return js;
}

json_node* parse_json(stream* rs, size_t max_json_string_length, size_t max_json_number_length, int* error)
{
	lexer lxr;
	if(!initialize_lexer(&lxr, rs, max_json_string_length, max_json_number_length))
	{
		(*error) = JSON_LEXER_ERROR;
		return NULL;
	}

	(*error) = JSON_NO_ERROR;
	lexeme lxm;

	json_node* js = NULL;

	if(((*error) = get_next_lexeme_from_lexer(&lxr, &lxm)))
		goto EXIT;

	if(lxm.type == CURLY_OPEN_BRACE)
	{
		undo_lexer(&lxr, &lxm);
		js = parse_json_object_node(&lxr, error);
	}
	else if(lxm.type == SQUARE_OPEN_BRACE)
	{
		undo_lexer(&lxr, &lxm);
		js = parse_json_array_node(&lxr, error);
	}
	else
	{
		(*error) = JSON_PARSER_ERROR;
		destroy_lexeme(&lxm);
	}


	EXIT:;
	deinitialize_lexer(&lxr);
	return js;
}