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

	// build the iterator
	char* inst = json_string->cstring;
	dstring* temp_dstr = get_dstring(40);

	json_node* return_node = NULL;

	while((*inst) != '\0')
	{
		switch(*inst)
		{
			case '\"' :
			{
				int no_error = 1;

				// if it is starting of the reading of a string,
				// it can be a key for a json object
				if((*get_current_state(state_stack)) == READING_OBJECT)
				{
					push_state(state_stack, READING_KEY, NULL);
				}
				// or a string element of an array
				else if((*get_current_state(state_stack)) == READING_ARRAY)
				{
					push_state(state_stack, READING_STRING, get_new_json_node());
				}
				// or a value to be read, for an array of object
				else if((*get_current_state(state_stack)) == VALUE_TO_BE_READ)
				{
					pop_state(state_stack);
					push_state(state_stack, READING_STRING, get_new_json_node());
				}
				else if((*get_current_state(state_stack)) == READING_STRING)
				{
					pop_state(state_stack);
				}
				else if((*get_current_state(state_stack)) == READING_KEY)
				{
					pop_state(state_stack);
					push_state(state_stack, KEY_PARSED, NULL);
				}
				else
				{
					// ERROR
					no_error = 0;
				}

				if(no_error)
				{
					if(temp_dstr == NULL)
					{
						temp_dstr = get_dstring(40);
					}
				}
				break;
			}
			case ':' :
			{
				if((*get_current_state(state_stack)) == KEY_PARSED)
				{
					pop_state(state_stack);
					push_state(state_stack, VALUE_TO_BE_READ, NULL);

					// add a new element in the object, and update jnode_p
					insert_entry(get_current_state_reinstate_node(state_stack), read_str, get_new_json_node());

					// increment the elements read
					increment_current_state_elements_read(state_stack);
				}
				else
				{
					// ERROR
				}
				break;
			}
			case ',' :
			{
				if((*get_current_state(state_stack)) == READING_OBJECT || (*get_current_state(state_stack)) == READING_ARRAY)
				{
					// increment the number of elements in the current state, either array or hashmap
					unsigned long long int index = increment_current_state_elements_read(state_stack);

					// add a new array element if it is an ARRAY
					if((*get_current_state(state_stack)) == READING_ARRAY)
					{
						// create a new array element and save it in array, and update jnode_p
						json_node* new_jnode_p = get_new_json_node();
						set_element(get_current_state_reinstate_node(state_stack), new_jnode_p, index);
						jnode_p = new_jnode_p;
					}
				}
				else
				{
					// ERROR
				}
				break;
			}
			case '{' :
			{
				// insert a new json node, on the stack and update the state to READING OBJECT
				push_state(state_stack, READING_OBJECT, get_new_json_node());

				// initialize the last added json_node to an object
				initialize_json_node(get_current_state_reinstate_node(state_stack), OBJECT, 30);
				break;
			}
			case '}' :
			{
				if((*get_current_state(state_stack)) == READING_OBJECT)
				{
					// we need to return the outer most object
					return_node = pop_state(state_stack);
				}
				else
				{
					// ERROR
				}
				break;
			}
			case '[' :
			{
				// insert a new json node, on the stack and update the state to READING OBJECT
				push_state(state_stack, READING_ARRAY, get_new_json_node());

				// initialize the last added json_node to an array
				initialize_json_node(get_current_state_reinstate_node(state_stack), ARRAY, 30);
				break;
			}
			case ']' :
			{
				if((*get_current_state(state_stack)) == READING_ARRAY)
				{
					// we need to return the outer most array
					return_node = pop_state(state_stack);
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
					append_to_dstring(temp_dstr, temp_cstring);
				}
				break;
			}
		}
		inst++;
	}

	if(temp_dstr != NULL)
	{
		delete_dstring(temp_dstr);
	}

	// delete all the contents of the stack and the stack itself
	delete_state_stack(state_stack);

	return json_parent_most_node_p;
}