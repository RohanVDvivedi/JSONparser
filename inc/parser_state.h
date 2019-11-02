#ifndef PARSER_STATE_H
#define PARSER_STATE_H

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

#endif