#include<json_parser.h>

#include<json_lexer.h>

#include<stdio.h>

static json_node* parse_json_node(lexer* lxr)
{
	json_node* js = NULL;

	lexeme lxm;

	// read the next lexeme to ch
	if(!get_next_lexeme_from_lexer(lxr, &lxm))
		goto FAIL;

	switch(lxm.type)
	{
		// TODO
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