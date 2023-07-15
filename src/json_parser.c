#include<json_parser.h>

#include<json_lexer.h>

#include<stdio.h>

json_node* parse_json(stream* rs, size_t max_json_string_length, size_t max_json_number_length)
{
	lexer lxr;
	initialize_lexer(&lxr, rs, max_json_string_length, max_json_number_length);

	lexeme lxm;
	while(1)
	{
		if(!get_next_lexeme_from_lexer(&lxr, &lxm))
		{
			printf("ERROR\n");
			break;
		}

		switch(lxm.type)
		{
			case END_OF_STREAM:
			{
				printf("END_OF_STREAM\n");
				break;
			}
			case CURLY_OPEN_BRACE :
			{
				printf("CURLY_OPEN_BRACE\n");
				break;
			}
			case CURLY_CLOSE_BRACE :
			{
				printf("CURLY_CLOSE_BRACE\n");
				break;
			}
			case SQUARE_OPEN_BRACE :
			{
				printf("SQUARE_OPEN_BRACE\n");
				break;
			}
			case SQUARE_CLOSE_BRACE :
			{
				printf("SQUARE_CLOSE_BRACE\n");
				break;
			}
			case COMMA :
			{
				printf("COMMA\n");
				break;
			}
			case COLON :
			{
				printf("COLON\n");
				break;
			}
			case TRUE :
			{
				printf("TRUE\n");
				break;
			}
			case FALSE :
			{
				printf("FALSE\n");
				break;
			}
			case NULL_LEXEME :
			{
				printf("NULL_LEXEME\n");
				break;
			}
			case STRING_LEXEME :
			{
				printf("STRING_LEXEME : <"printf_dstring_format">\n", printf_dstring_params(&(lxm.lexeme_str)));
				break;
			}
			case NUMBER_LEXEME :
			{
				printf("NUMBER_LEXEME : <"printf_dstring_format">\n", printf_dstring_params(&(lxm.lexeme_str)));
				break;
			}
		}

		if(lxm.type == END_OF_STREAM)
			break;
	}

	deinitialize_lexer(&lxr);
	return NULL;
}