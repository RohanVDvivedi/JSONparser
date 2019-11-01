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
				int no_error = 1;

				if((*get_current_state(state_stack)) == READING_OBJECT)
				{
					push_state(state_stack, READING_KEY);

					// add a new element in the object
					
				}
				else if((*get_current_state(state_stack)) == VALUE_TO_BE_READ || (*get_current_state(state_stack)) == READING_ARRAY)
				{
					push_state(state_stack, READING_STRING);
				}
				else if((*get_current_state(state_stack)) == READING_STRING)
				{
					pop_state(state_stack);
				}
				else if((*get_current_state(state_stack)) == READING_KEY)
				{
					pop_state(state_stack);
					push_state(state_stack, KEY_PARSED);
				}
				else
				{
					// ERROR
					no_error = 0;
				}

				if(no_error)
				{

				}
				break;
			}
			case ':' :
			{
				if((*get_current_state(state_stack)) == KEY_PARSED)
				{
					pop_state(state_stack);
					push_state(state_stack, VALUE_TO_BE_READ);
				}
				else
				{
					// ERROR
				}
				break;
			}
			case ',' :
			{
				if((*get_current_state(state_stack)) == READING_OBJECT || (*get_current_state(state_stack)) == READING_NUMBER)
				{
					// increment the number of elements in the current state, either array or hashmap
					unsigned long long int index = increment_current_state_elements_read(state_stack);

					// add a new element if it is an ARRAY
					if
				}
				else
				{
					// ERROR
				}
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
				else
				{
					// ERROR
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
				else
				{
					// ERROR
				}
				break;
			}
			// loop over the elements from the characters, reading and completing, a string, number, boolean or null
			default :
			{
				// \ is escape character
				if(*inst == '\\' && ((*get_current_state(state_stack)) == READING_STRING || (*get_current_state(state_stack)) == READING_KEY))
				{
					inst++;
					// handle escaped character
					switch(*inst)
					{
						default :
						{
							break;
						}
					}
					break;
				}
				else
				{
					// make a string from the character and append it to the dstring
					char temp_cstring[2] = "Z";
					temp_cstring[0] = *inst;
					append_to_dstring(jnode_p->data_p, temp_cstring);
				}
				break;
			}
		}
		inst++;
	}

	// delete all the contents of the stack and the stack itself
	delete_state_stack(state_stack);

	return json_parent_most_node_p;
}