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
	json_node* jnode_p = get_new_json_node();
	json_node* json_parent_most_node_p = jnode_p;

	char* inst = json_string->cstring;
	while((*inst) != '\0')
	{
		switch(*inst)
		{
			case '\"' :
			{
				if((*get_current_state(state_stack)) == READING_OBJECT)
				{
					push_state(state_stack, READING_KEY);
				}
				else if((*get_current_state(state_stack)) == READING_OBJECT)
				{
					push_state(state_stack, READING_STRING);
				}
				else if((*get_current_state(state_stack)) == READING_STRING || (*get_current_state(state_stack)) == READING_KEY)
				{
					pop_state(state_stack);
				}
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
				push_state(state_stack, READING_OBJECT);
				initialize_json_node(jnode_p, OBJECT, 30);
				break;
			}
			case '}' :
			{
				if((*get_current_state(state_stack)) == READING_OBJECT)
				{
					pop_state(state_stack);
				}
				break;
			}
			case '[' :
			{
				push_state(state_stack, READING_ARRAY);
				initialize_json_node(jnode_p, ARRAY, 30);
				break;
			}
			case ']' :
			{
				if((*get_current_state(state_stack)) == READING_ARRAY)
				{
					pop_state(state_stack);
				}
				break;
			}
			default  :
			{
				break;
			}
		}
		inst++;
	}

	// delete all the contents of the stack and the stack itself
	delete_state_stack(state_stack);

	return json_parent_most_node_p;
}