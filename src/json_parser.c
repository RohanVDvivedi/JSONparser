#include<json_parser.h>

void print_json_node(json_node* jnodep)
{
	if(jnodep == NULL)
	{
		printf("NODE IS NULL\n");
		return;
	}
	else if(jnodep->type == ARRAY)
	{
		print_array(((array*)(jnodep->data_p)), print_json_node);
	}
	else if(jnodep->type == ARRAY)
	{
		print_hashmap(((hashmap*)(jnodep->data_p)), print_json_node, print_json_node);
	}
	else
	{
		printf("%d => %s\n", jnodep->type, ((dstring*)(jnodep->data_p))->cstring);
	}
}

json_node* parse_json(dstring* json_string)
{
	// create a state stack
	stack* state_stack = get_stack((json_string->bytes_occupied / 10) + 10);

	// build the iterator
	char* inst = json_string->cstring;

	json_node* return_node = NULL;

	while((*inst) != '\0')
	{
		switch(*inst)
		{
			case '\"' :
			{
				// if it is starting of the reading of a string,
				// it can be a key for a json object
				if(get_current_state(state_stack) == READING_OBJECT)
				{
					push_state(state_stack, READING_KEY, get_new_json_node());

					// initialize the new STRING json_node to 10 characters
					initialize_json_node(get_current_state_reinstate_node(state_stack), STRING, 10);
				}
				// or a string element of an array
				else if(get_current_state(state_stack) == READING_ARRAY)
				{
					push_state(state_stack, READING_STRING, get_new_json_node());

					// initialize the new STRING json_node to 10 characters
					initialize_json_node(get_current_state_reinstate_node(state_stack), STRING, 10);
				}
				// or a value to be read, for an array of object
				else if(get_current_state(state_stack) == VALUE_TO_BE_READ)
				{
					// pop the VALUE_TO_BE_READ, because we have got to read the STRING now
					pop_state(state_stack);
					push_state(state_stack, READING_STRING, get_new_json_node());

					// initialize the new STRING json_node to 10 characters
					initialize_json_node(get_current_state_reinstate_node(state_stack), STRING, 10);
				}

				else if(get_current_state(state_stack) == READING_STRING)
				{
					pop_state(state_stack);
				}
				else if(get_current_state(state_stack) == READING_KEY)
				{
					push_state(state_stack, KEY_PARSED, NULL);
				}
				else
				{
					// ERROR
				}
				break;
			}
			case ':' :
			{
				if(get_current_state(state_stack) == KEY_PARSED)
				{
					pop_state(state_stack);
					push_state(state_stack, VALUE_TO_BE_READ, NULL);
				}
				else
				{
					// ERROR
				}
				break;
			}
			case ',' :
			{
				// only after reading valid data value, we can add it to an array or an object
				if(
						get_current_state(state_stack) == READING_STRING
					||	get_current_state(state_stack) == READING_ARRAY
					||	get_current_state(state_stack) == READING_OBJECT
					||	get_current_state(state_stack) == READING_RAW_DATA
					)
				{
					json_node* value = pop_state(state_stack);
					if(get_current_state(state_stack) == READING_KEY)
					{
						json_node* key = pop_state(state_stack);
						if(get_current_state(state_stack) == READING_OBJECT)
						{
							// increment the number of elements in the current state, either array or hashmap
							increment_current_state_elements_read(state_stack);

							// insert entry in the hashmap
							insert_entry_in_hash(((hashmap*)(((json_node*)get_current_state_reinstate_node(state_stack))->data_p)), key, value);
						}
						else
						{
							// ERROR
						}
					}
					else if(get_current_state(state_stack) == READING_ARRAY)
					{
						// increment the number of elements in the current state, either array or hashmap
						unsigned long long int index = increment_current_state_elements_read(state_stack);

						// create a new array element and save it in array
						set_element(((array*)(((json_node*)get_current_state_reinstate_node(state_stack))->data_p)), value, index);
					}
					else
					{
						// ERROR
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
				if(get_current_state(state_stack) == VALUE_TO_BE_READ)
				{
					pop_state(state_stack);
				}

				// insert a new json node, on the stack and update the state to READING OBJECT
				push_state(state_stack, READING_OBJECT, get_new_json_node());

				// initialize the last added json_node to an object
				initialize_json_node(get_current_state_reinstate_node(state_stack), OBJECT, 30);
				break;
			}
			case '}' :
			{
				// the value for the key is going to be an array
				if(get_current_state(state_stack) == READING_OBJECT)
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
				// the value for the key is going to be an array
				if(get_current_state(state_stack) == VALUE_TO_BE_READ)
				{
					pop_state(state_stack);
				}

				// insert a new json node, on the stack and update the state to READING OBJECT
				push_state(state_stack, READING_ARRAY, get_new_json_node());

				// initialize the last added json_node to an array
				initialize_json_node(get_current_state_reinstate_node(state_stack), ARRAY, 30);
				break;
			}
			case ']' :
			{
				if(get_current_state(state_stack) == READING_ARRAY)
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
				if(*inst == '\\' && (get_current_state(state_stack) == READING_STRING || get_current_state(state_stack) == READING_KEY))
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
				// if it is some json node that is a dstring, we append the new data at its end
				else if( 	
							get_current_state(state_stack) == READING_STRING
						||	get_current_state(state_stack) == READING_KEY
						||	get_current_state(state_stack) == READING_RAW_DATA 
					)
				{
					// make a string from the character and append it to the dstring
					char temp_cstring[2] = "Z";
					temp_cstring[0] = *inst;
					append_to_dstring((dstring*)(((json_node*)get_current_state_reinstate_node(state_stack))->data_p), temp_cstring);
				}
				else if(get_current_state(state_stack) == VALUE_TO_BE_READ)
				{
					// do nothing just skip
					if(*inst == ' ' || *inst == '\r' || *inst == '\n' || *inst == '\t')
					{}
					else
					{
						// now ther has to be a node where we can read
						pop_state(state_stack);

						push_state(state_stack, READING_RAW_DATA, get_new_json_node());

						// initialize it to an ERROR
						initialize_json_node(get_current_state_reinstate_node(state_stack), ERROR, 10);
					}
				}
				else
				{
					// ERROR
				}
				break;
			}
		}
		inst++;
	}

	// delete all the contents of the stack and the stack itself
	delete_state_stack(state_stack);

	return return_node;
}