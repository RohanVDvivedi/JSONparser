#include<json_parser.h>

void start_array_reading(stack* state_stack)
{
	// having an EMPTY node of type VALUE_TO_BE_READ only specifies that some other object is waiting for this array to be formed completely
	if(is_current_state_equals(state_stack, VALUE_TO_BE_READ))
	{
		pop_state(state_stack);
	}

	// insert a new json node, on the stack and update the state to READING OBJECT
	push_state(state_stack, READING_ARRAY, get_new_json_node());

	// initialize the last added json_node to an array
	initialize_json_node(get_current_state_reinstate_node(state_stack), ARRAY, 30);
}

void start_object_reading(stack* state_stack)
{
	// having an EMPTY node of type VALUE_TO_BE_READ only specifies that some other object is waiting for this object to be formed completely
	if(is_current_state_equals(state_stack, VALUE_TO_BE_READ))
	{
		pop_state(state_stack);
	}

	// insert a new json node, on the stack and update the state to READING OBJECT
	push_state(state_stack, READING_OBJECT, get_new_json_node());

	// initialize the last added json_node to an object
	initialize_json_node(get_current_state_reinstate_node(state_stack), OBJECT, 30);
}

void start_raw_data_reading(stack* state_stack)
{
	// if we have been asked to read Value, we first pop the empty state
	if(is_current_state_equals(state_stack, VALUE_TO_BE_READ))
	{
		pop_state(state_stack);
	}

	// now ther has to be a node where we can read
	push_state(state_stack, READING_RAW_DATA, get_new_json_node());

	// initialize it to an ERROR
	initialize_json_node(get_current_state_reinstate_node(state_stack), ERROR, 10);
}

void start_string_reading(stack* state_stack)
{
	parse_state string_state;
	if(is_current_state_equals(state_stack, READING_OBJECT))
	{
		string_state = READING_KEY;
	}
	// or a string element of an array
	else if(is_current_state_equals(state_stack, READING_ARRAY))
	{
		string_state = READING_STRING;
	}
	// or a value to be read, for an array of object
	else if(is_current_state_equals(state_stack, VALUE_TO_BE_READ))
	{
		// pop the VALUE_TO_BE_READ, because we have got to read the STRING now
		pop_state(state_stack);
		string_state = READING_STRING;
	}
	else
	{
		// ERROR
	}

	push_state(state_stack, string_state, get_new_json_node());

	// initialize the new STRING json_node to 10 characters
	initialize_json_node(get_current_state_reinstate_node(state_stack), STRING, 10);
}

typedef enum operation_type operation_type;
enum operation_type
{
	INSERT_ENTRY_IN_OBJECT   = 0x01,
	APPEND_ELEMENT_IN_ARRAY  = 0x10
};

void perform_composite_operation(stack* state_stack, operation_type optype)
{
	// only after reading valid data value, we can add it to an array or an object
	if(is_current_state_equals(state_stack, READING_COMPLETE))
	{
		// pop the state of READING_COMPLETE
		pop_state(state_stack);

		// pop and collect the value node whose reading was completed
		json_node* value = pop_state(state_stack);
		if( (optype & INSERT_ENTRY_IN_OBJECT) && is_current_state_equals(state_stack, READING_KEY))
		{
			// pop and collect the key node whose reading was completed
			json_node* key = pop_state(state_stack);
			if(is_current_state_equals(state_stack, READING_OBJECT))
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
		else if( (optype & APPEND_ELEMENT_IN_ARRAY) && is_current_state_equals(state_stack, READING_ARRAY))
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
}

json_node* complete_array_reading(stack* state_stack)
{
	if(is_current_state_equals(state_stack, READING_ARRAY))
	{
		if(state_stack->stack_size > 1)
		{
			// push a state mentioning that the reading is complete for the current json array
			push_state(state_stack, READING_COMPLETE, NULL);
		}
		else
		{
			return pop_state(state_stack);
		}
	}
	else
	{
		// ERROR
	}
	return NULL;
}

json_node* complete_object_reading(stack* state_stack)
{
	// the value for the key is going to be an array
	if(is_current_state_equals(state_stack, READING_OBJECT))
	{
		if(state_stack->stack_size > 1)
		{
			// push a state mentioning that the reading is complete for the current json object
			push_state(state_stack, READING_COMPLETE, NULL);
		}
		else
		{
			return pop_state(state_stack);
		}
	}
	else
	{
		// ERROR
	}
	return NULL;
}

void complete_raw_data_reading(stack* state_stack)
{
	// if it is a READING_RAW_DATA state, just push READING_COMPLETE, so notify that a valid value has been read
	if(is_current_state_equals(state_stack, READING_RAW_DATA))
	{
		// identify the RAW_DATA it can be any of BOOLE, NUMBER, NULLE or ERROR
		identify_dstring_json_node(((json_node*)get_current_state_reinstate_node(state_stack)));

		// push an empty state of reading complete
		push_state(state_stack, READING_COMPLETE, NULL);
	}
}

void complete_string_reading(stack* state_stack)
{
	if(is_current_state_equals(state_stack, READING_STRING))
	{
		push_state(state_stack, READING_COMPLETE, NULL);
	}
	else if(is_current_state_equals(state_stack, READING_KEY))
	{
		push_state(state_stack, KEY_PARSED, NULL);
	}
	else
	{
		// ERROR
	}
}

void append_character(stack* state_stack, char to_append)
{
	if( is_current_state_equals(state_stack, READING_STRING)
		|| is_current_state_equals(state_stack, READING_KEY)
		|| is_current_state_equals(state_stack, READING_RAW_DATA) )
	{
		// make a string from the character
		char temp_cstring[2] = "Z"; temp_cstring[0] = to_append;

		// and append it to the dstring
		append_to_dstring((dstring*)(((json_node*)get_current_state_reinstate_node(state_stack))->data_p), temp_cstring);
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
				// or a string element of an array
				// or a value to be read, for a key of object
				if( is_current_state_equals(state_stack, READING_OBJECT)
					|| is_current_state_equals(state_stack, READING_ARRAY)
					|| is_current_state_equals(state_stack, VALUE_TO_BE_READ) )
				{
					start_string_reading(state_stack);
				}

				// if the STRING is already being read, either as a READING_STRING or READING_KEY state
				// the string has to be terminated
				else if( is_current_state_equals(state_stack, READING_STRING)
						 || is_current_state_equals(state_stack, READING_KEY) )
				{
					complete_string_reading(state_stack);
				}
				
				break;
			}
			case ':' :
			{
				if(is_current_state_equals(state_stack, KEY_PARSED))
				{
					// pop the empty KEY_PARSED state
					pop_state(state_stack);

					// push the empty state that asks to read the value for the KEY_PARSED
					push_state(state_stack, VALUE_TO_BE_READ, NULL);
				}
				else
				{
					// ERROR
				}
				break;
			}
			case '{' :
			{
				start_object_reading(state_stack);
				break;
			}
			case '[' :
			{
				start_array_reading(state_stack);
				break;
			}
			case ',' :
			{
				// comma ends a NUMBER, BOOLE or NULLE data, hence if we were expecting any of them to continue, 
				// we were wrong, just push READING_COMPLETE, so notify that a valid value has been read
				complete_raw_data_reading(state_stack);

				// only after reading valid data value, we can add it to an array or an object
				perform_composite_operation(state_stack, INSERT_ENTRY_IN_OBJECT | APPEND_ELEMENT_IN_ARRAY);
				break;
			}
			case '}' :
			{
				// comma ends a NUMBER, BOOLE or NULLE data, hence if we were expected any of them to continue, 
				// we were wrong, just push READING_COMPLETE, so notify that a valid value has been read
				complete_raw_data_reading(state_stack);

				// read and push in the last element of the json object
				perform_composite_operation(state_stack, INSERT_ENTRY_IN_OBJECT);

				return_node = complete_object_reading(state_stack);
				break;
			}
			case ']' :
			{
				// comma ends a NUMBER, BOOLE or NULLE data, hence if we were expected any of them to continue, 
				// we were wrong, just push READING_COMPLETE, so notify that a valid value has been read
				complete_raw_data_reading(state_stack);

				// read and push in the last element of the json array
				perform_composite_operation(state_stack, APPEND_ELEMENT_IN_ARRAY);

				return_node = complete_array_reading(state_stack);
				break;
			}
			// handling escape chacters
			case '\\' :
			{
				append_character(state_stack, *inst); inst++;
				append_character(state_stack, *inst);
			}
			// loop over the elements from the characters, reading and completing, a string, number, boolean or null
			default :
			{
				// this is where a new READING_RAW_DATA state may start,
				// when the VALUE_TO_BE_READ or READING_ARRAY and we are not looking at any of white spaces
				if((is_current_state_equals(state_stack, VALUE_TO_BE_READ) || is_current_state_equals(state_stack, READING_ARRAY)) 
						&& (!(*inst == ' ' || *inst == '\r' || *inst == '\n' || *inst == '\t')) )
				{
					start_raw_data_reading(state_stack);
				}
				// READING_RAW_DATA is terminated by any white space character
				else if(is_current_state_equals(state_stack, READING_RAW_DATA) 
					&& (*inst == ' ' || *inst == '\r' || *inst == '\n' || *inst == '\t'))
				{
					complete_raw_data_reading(state_stack);
				}

				// if it is some json node that is a dstring, we append the new data at its end
				append_character(state_stack, *inst);
				break;
			}
		}
		inst++;
	}

	// delete all the contents of the stack and the stack itself
	delete_state_stack(state_stack);

	return return_node;
}