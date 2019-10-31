#include<json_parser.h>

// returns character pointer pointing to the next character from the *from and that equals the matched character
char* get_until(char* from, char matches)
{
	char* result = from;
	while((*result) != matches && (*result) != '\0')
	{
		result++;
	}
	return ((*result) == matches) ? result : NULL;
}

json_node* parse_json(dstring* json_string)
{
	// create a state stack
	stack* state_stack = get_stack((json_string->bytes_occupied / 10) + 10);

	// this is what the result will pointed to by
	json_node* jnode = NULL;

	char* inst = json_string->cstring;
	while((*inst) != '\0')
	{
		switch(*inst)
		{
			case '\"' :
			{
				break;
			}
			case ':' :
			{
				break;
			}
			case ',' :
			{
				break;
			}
			case '{' :
			{
				break;
			}
			case '}' :
			{
				break;
			}
			case '[' :
			{
				break;
			}
			case ']' :
			{
				break;
			}
		}
		inst++;
	}

	// delete all the contents of the stack and the stack itself
	delete_state_stack(state_stack);

	return jnode;
}

void delete_json(json_node* jnode)
{

}