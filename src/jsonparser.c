#include<jsonparser.h>

typedef enum parse_state parse_state;
enum parse_state
{
	READING_KEY,
	KEY_PARSED,
	READING_STRING,
	READING_NUMBER,
	READING_ARRAY,
	READING_OBJECT
};

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


	// delete all the contents of the stack and the stack itself
	while(state_stack->stack_size > 0)
	{
		void* element = (void*)get_top_stack(state_stack);
		free(element);
		pop_stack(state_stack);
	}
	delete_stack(state_stack);

	return jnode;
}