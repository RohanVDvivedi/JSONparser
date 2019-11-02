#ifndef STATE_STACK_H
#define STATE_STACK_H

#include<stdio.h>
#include<stdlib.h>

#include<dstring.h>
#include<stack.h>

#include<json_node.h>

typedef enum parse_state parse_state;
enum parse_state
{
	READING_KEY,				// this state is pushed when we encounter ", and the previous state is READING_OBJECT

	KEY_PARSED,					// this state is pushed when we encounter " and the previous state is READING_ENTRY
								// this is an empty state, that specifies that the READING_KEY state is completed

	VALUE_TO_BE_READ,			// empty state that gets replaced after KEY_PARSED state, when : is encountered

	READING_RAW_DATA,			// if the data is not identified yet, it can be a NUMBER, BOOLE or NULLE
	READING_STRING,				// this state is pushed when we encounter "
	READING_ARRAY,				// this state is pushed when we encounter [
	READING_OBJECT,				// this state is pushed when we encounter {

	READING_COMPLETE			// this state is pushed when we encounter " or [ or } and 
								// the previous state is any of READING_OBJECT or READING_STRING or READING_ARRAY
								// this is an empty state specifying that NUMBER, BOOLE, NULLE, STRING, ARRAY, OBJECT has been read and parsed successfully
};

typedef struct state_desc state_desc;
struct state_desc
{
	parse_state state;

	json_node* reinstate_to_node;

	unsigned long long int elements_read;
};

// gets a new state stack for the given json string
stack* get_state_stack(dstring* json_string);

// returns 1 if the current state of state stack equals the state as passed in the parameter
int is_current_state_equals(stack* state_stack, parse_state state);

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