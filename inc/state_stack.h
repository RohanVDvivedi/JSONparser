#ifndef STATE_STACK_H
#define STATE_STACK_H

#include<stdio.h>
#include<stdlib.h>

#include<dstring.h>
#include<stack.h>

typedef enum parse_state parse_state;
enum parse_state
{
	READING_KEY,
	KEY_PARSED,
	VALUE_TO_BE_READ,
	READING_STRING,
	READING_NUMBER,
	READING_NULLE,
	READING_BOOLE,
	READING_ARRAY,
	READING_OBJECT
};

typedef struct state_desc state_desc;
struct state_desc
{
	parse_state state;

	json_node* reinstate_to_node;

	unsigned long long int elements_read;
}

// gets a new state stack for the given json string
stack* get_state_stack(dstring* json_string);

// gets the currentr state pf the parser from the given state
parse_state* get_current_state(stack* state_stack);

// gives you the current, json node, to be reinstated for the last state that was pushed
json_node* get_current_state_reinstate_node(stack* state_stack);

// incremnts, the number of elements read when in current state, i.e. the state at the top of the stack
// and returns the incremented value
unsigned long long int increment_current_state_elements_read(stack* state_stack);

// pushes a new state to the given state stack
// along with the state you also push the current parent json node that you are playing with
// this can be used to reinstate the json_node pointer functionality, in the parser
void push_state(stack* state_stack, parse_state state, json_node* reinstate_to_node);

// removes the top most element from the state stack
// it returns the json_node, which was pushed with the push of the given state,
// this can be used to reinstate the json_node pinter functionality, in the parser
json_node* pop_state(stack* state_stack);

// deletes all of the state stack, and all its elements freed and popped
void delete_state_stack(stack* state_stack);

#endif