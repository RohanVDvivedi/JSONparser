#include<stdio.h>
#include<string.h>

#include<json_lexer.h>
#include<json_parser.h>
#include<json_serializer.h>

int main()
{
	printf("json string building start\n\n");

	dstring const * const json_str = dstring_DUMMY_CSTRING("{ \
		\"name\" : \"Rohan\", \
		\"age\" : 23, \
		\"smart\" : true, \
		\"dependents\" : null, \
		\"family membe\" : [[1,2,3,4] \
		,[\"1\",\"2\",\"3\",\"4\"]],\
		\"nick name\" : \"Rohu\", \
		\"age next year\" : 24, \
		\"IQ\" : 196, \
		\"coder\" : true, \
		\"is dependent\" : false \
	}");

	printf("String built : ");
	printf_dstring(json_str);
	printf("\n\n");

	json_lexer jslx;
	init_json_lexer(&jslx, (dstring*)json_str);

	json_lexeme jslxm;
	int success = 1;
	do
	{
		success = get_next_json_lexeme(&jslx, &jslxm);
		printf("%d ", jslxm.type);
		if(jslxm.type != END_OF_JSON_STRING)
			printf_dstring(&(jslxm.value));
		printf("\n");
	}
	while(success && jslxm.type != END_OF_JSON_STRING);

	/*json_node* jnodep = parse_json(json_str);

	printf("Parsing done\n\n");
	
	print_json_node(jnodep);

	printf("Printing done\n\n");

	dstring result;init_dstring(&result, NULL, 0);
	serialize_json(&result, jnodep);
	printf("json string : <");
	printf_dstring(&result);
	printf(">\n");
	deinit_dstring(&result);

	printf("serializing done\n\n");

	if(jnodep != NULL)
	{
		delete_json_node(jnodep);

		printf("Json node deleted\n\n");
	}

	printf("deletion of dstring\n\n");
	return 0;*/
}