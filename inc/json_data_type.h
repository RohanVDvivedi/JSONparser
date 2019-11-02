#ifndef JSON_DATA_TYPE_H
#define JSON_DATA_TYPE_H

typedef enum json_data_type json_data_type;
enum json_data_type
{
	NULLE = 0,		// dstring	(null)
	BOOLE = 1,		// dstring  (true, false)
	NUMBER = 2,		// dstring  ([-][0-9][.][0-9])
	STRING = 3,		// dstring  (*)
	ARRAY = 4,		// array    (standard cutlery array of json_node*)
	OBJECT = 5,		// hashmap  (standard cutlery hashmap of json_node*(STRING mandatorily) vs json_node*)

	ERROR = 6		// this is when the json string has error and is not valid and can not be parsed
					// ERROR dstring (*)
};

#endif