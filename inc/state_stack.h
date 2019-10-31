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
	READING_STRING,
	READING_NUMBER,
	READING_ARRAY,
	READING_OBJECT
};

// gets a new state stack for the given json string
stack* get_state_stack(dstring* json_string);

// gets the currentr state pf the parser from the given state
parse_state* get_current_state(stack* state_stack);

// pushes a new state to the given state stack
void push_state(stack* state_stack, parse_state state);

// removes the top most element from the state stack
void pop_state(stack* state_stack);

// deletes all of the state stack, and all its elements freed and popped
void delete_state_stack(stack* state_stack);

#endif