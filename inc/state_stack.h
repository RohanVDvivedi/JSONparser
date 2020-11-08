#ifndef STATE_STACK_H
#define STATE_STACK_H

#include<dstring.h>
#include<stack.h>

#include<json_node.h>
#include<parser_state.h>

typedef struct state_desc state_desc;
struct state_desc
{
	parse_state state;

	json_node* reinstate_to_node;

	unsigned int elements_read;
};

// initializes as a new state stack for the given json string
void initialize_state_stack(stack* state_stack, const dstring* json_string);

// returns 1 if the current state of state stack equals the state as passed in the parameter
int is_current_state_equals(stack* state_stack, parse_state state);

// gives you the current, json node, to be reinstated for the last state that was pushed
json_node* get_current_state_reinstate_node(stack* state_stack);

// incremnts, the number of elements read when in current state, i.e. the state at the top of the stack
// and returns the incremented value
unsigned int increment_current_state_elements_read(stack* state_stack);

// pushes a new state to the given state stack
// along with the state you also push the current parent json node that you are playing with
// this can be used to reinstate the json_node pointer functionality, in the parser
void push_state(stack* state_stack, parse_state state, json_node* reinstate_to_node);

// removes the top most element from the state stack
// it returns the json_node, which was pushed with the push of the given state,
// this can be used to reinstate the json_node pinter functionality, in the parser
json_node* pop_state(stack* state_stack);

void deinitialize_state_stack(stack* state_stack);

#endif