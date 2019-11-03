#ifndef JSON_DATA_TYPE_H
#define JSON_DATA_TYPE_H

typedef enum json_data_type json_data_type;
enum json_data_type
{
	NULLE = 0,		// dstring	(null)
	BOOLE = 1,		// dstring  (true, false)
	NUMBER = 2,		// dstring  ([-][0-9][.][0-9])
	STRING = 3,		// dstring  (*)
	KEY = 4,		// dstring  (*) same as STRING, but this can only be inside an object, and map to any other data type
	ARRAY = 5,		// array    (standard cutlery array of json_node*)
	OBJECT = 6,		// hashmap  (standard cutlery hashmap of json_node*(KEY mandatorily) vs json_node*)

	ERROR = 7		// this is when the json string has error and is not valid and can not be parsed
					// ERROR dstring (*)
};

#endif