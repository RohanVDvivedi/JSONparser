#include<state_stack.h>

/*
	stack state object
*/

// create function
state_desc* get_new_state_desc(parse_state state, json_node* reinstate_to_node)
{
	state_desc* desc = (state_desc*) malloc(sizeof(state_desc));
	desc->state = state;
	desc->reinstate_to_node = reinstate_to_node;
	desc->elements_read = 0;
	return desc;
}

/*
	stack state object
*/

void initialize_state_stack(stack* state_stack, dstring* json_string)
{
	initialize_stack(state_stack, (json_string->bytes_occupied / 10) + 10);
}

int is_current_state_equals(stack* state_stack, parse_state state)
{
	// return state of the state_desc that is on top of the stack
	return state_stack->stack_size > 0 ? ((state_desc*)get_top_stack(state_stack))->state == state : 0;
}

json_node* get_current_state_reinstate_node(stack* state_stack)
{
	// return reinstate node pointer of the state_desc that is on top of the stack
	return ((state_desc*)get_top_stack(state_stack))->reinstate_to_node;
}

unsigned long long int increment_current_state_elements_read(stack* state_stack)
{
	return ((state_desc*)get_top_stack(state_stack))->elements_read++;
}

void push_state(stack* state_stack, parse_state state, json_node* reinstate_to_node)
{
	// create a new element that can be pushed to the stack
	state_desc* new_state_desc = get_new_state_desc(state, reinstate_to_node);

	// push it on
	push_stack(state_stack, new_state_desc);
}

json_node* pop_state(stack* state_stack)
{
	// get the first element and free it
	state_desc* desc = (state_desc*)get_top_stack(state_stack);

	// get the reinstate node value, so that it can be returned
	json_node* reinstate_to_node = desc->reinstate_to_node;

	// free acquired memory
	free(desc);

	// pop the freed element from the stack
	pop_stack(state_stack);

	// return the json node pointer so that it can be used to fetch other elements
	return reinstate_to_node;
}

void deinitialize_state_stack(stack* state_stack)
{
	// delete/pop 
	while(state_stack->stack_size > 0)
	{
		pop_state(state_stack);
	}
	deinitialize_stack(state_stack);
}