#include<json_parser.h>

void start_reading(stack* state_stack, json_data_type data_type_to_expect)
{
	// if we have been asked to read Value, we first pop the empty state
	if(is_current_state_equals(state_stack, VALUE_TO_BE_READ) && data_type_to_expect != KEY)
	{
		pop_state(state_stack);
	}
	else if(is_current_state_equals(state_stack, VALUE_TO_BE_READ) && data_type_to_expect == KEY)
	{
		// ERROR
	}

	parse_state reading_state = get_parser_state_for(data_type_to_expect);

	// now there has to be a node where we can parse and put out data
	push_state(state_stack, reading_state, get_new_json_node());

	// initialize it to expected data type
	initialize_json_node(get_current_state_reinstate_node(state_stack), data_type_to_expect, 4);
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

				hashmap* hashmap_p = ((hashmap*)(((json_node*)get_current_state_reinstate_node(state_stack))->data_p));

				if(hashmap_p->bucket_count > 3 * hashmap_p->bucket_occupancy)
				{
					rehash_to_size(hashmap_p, 4 * hashmap_p->bucket_occupancy);
				}

				// insert entry in the hashmap
				insert_entry_in_hash(hashmap_p, key, value);
			}
			else
			{
				// ERROR
				return;
			}
		}
		else if( (optype & APPEND_ELEMENT_IN_ARRAY) && is_current_state_equals(state_stack, READING_ARRAY))
		{
			// increment the number of elements in the current state, either array or hashmap
			unsigned long long int index = increment_current_state_elements_read(state_stack);

			array* array_p = ((array*)(((json_node*)get_current_state_reinstate_node(state_stack))->data_p));

			// if that array is full we need to expand it
			if(index == array_p->total_size)
			{
				expand_array(array_p);
			}

			// create a new array element and save it in array
			set_element(array_p, value, index);
		}
		else
		{
			// ERROR
			return;
		}
	}
	else
	{
		// ERROR
		return;
	}
}

json_node* complete_array_object_reading(stack* state_stack, json_data_type data_type_to_expect)
{
	// what should be the expected state, to complete the data type to be expected to be completing
	parse_state expected_current_state = get_parser_state_for(data_type_to_expect);

	if(is_current_state_equals(state_stack, expected_current_state))
	{
		if(state_stack->stack_size > 1)
		{
			// push a state mentioning that the reading is complete for the current json array or object
			push_state(state_stack, READING_COMPLETE, NULL);
			return NULL;
		}
		// else return the array of json object since it can be an outer most object
		else
		{
			return pop_state(state_stack);
		}
	}
	else
	{
		// ERROR
		return NULL;
	}
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

void complete_string_or_key_reading(stack* state_stack)
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
		return;
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
				// if it is starting of the reading of a STRING or KEY,
				// it can be a key for a json object
				if(is_current_state_equals(state_stack, READING_OBJECT))
				{
					start_reading(state_stack, KEY);
				}
				// or a string element of an array
				// or a value to be read, for a key of object
				else if(is_current_state_equals(state_stack, READING_ARRAY)
					|| is_current_state_equals(state_stack, VALUE_TO_BE_READ) )
				{
					start_reading(state_stack, STRING);
				}

				// if the STRING or KEY is already being read,
				// either as a READING_STRING or READING_KEY state
				// the string has to be terminated
				else if( is_current_state_equals(state_stack, READING_STRING)
						 || is_current_state_equals(state_stack, READING_KEY) )
				{
					complete_string_or_key_reading(state_stack);
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
				start_reading(state_stack, OBJECT);
				break;
			}
			case '[' :
			{
				start_reading(state_stack, ARRAY);
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

				return_node = complete_array_object_reading(state_stack, OBJECT);
				break;
			}
			case ']' :
			{
				// comma ends a NUMBER, BOOLE or NULLE data, hence if we were expected any of them to continue, 
				// we were wrong, just push READING_COMPLETE, so notify that a valid value has been read
				complete_raw_data_reading(state_stack);

				// read and push in the last element of the json array
				perform_composite_operation(state_stack, APPEND_ELEMENT_IN_ARRAY);

				return_node = complete_array_object_reading(state_stack, ARRAY);
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
					start_reading(state_stack, ERROR);
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