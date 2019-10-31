#include<state_stack.h>

stack* get_state_stack(dstring* json_string)
{
	return get_stack((json_string->bytes_occupied / 10) + 10);
}

parse_state* get_current_state(stack* state_stack)
{
	// return the pointer to the same type casted element, as on top of the stack
	return (parse_state*)get_top_stack(state_stack);
}

void push_state(stack* state_stack, parse_state state)
{
	// create a new element that can be pished to the stack
	parse_state* new_state = (parse_state*) malloc(sizeof(parse_state));
	(*new_state) = state;

	// push it on
	push_stack(state_stack, new_state);
}

void pop_state(stack* state_stack)
{
	// get the first element and free it
	void* state_p = (void*)get_top_stack(state_stack);
	free(state_p);

	// pop the freed element from the stack
	pop_stack(state_stack);
}

void delete_state_stack(stack* state_stack)
{
	// delete/pop 
	while(state_stack->stack_size > 0)
	{
		pop_state(state_stack);
	}
	delete_stack(state_stack);
}