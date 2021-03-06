#include<parserstate_jsontype.h>

parse_state get_parser_state_for(json_data_type type)
{
	switch(type)
	{
		case ERROR :
		case NULLE :
		case BOOLE :
		case NUMBER :
		{
			return READING_RAW_DATA;
		}
		case STRING :
		{
			return READING_STRING;
		}
		case KEY :
		{
			return READING_KEY;
		}
		case ARRAY :
		{
			return READING_ARRAY;
		}
		case OBJECT :
		{
			return READING_OBJECT;
		}
	}
}