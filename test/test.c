#include<json_parser.h>
#include<json_serializer.h>

int main()
{
	printf("json string building start\n\n");

	dstring* json_str = get_dstring("\
	{ \
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
	} \
	", 10);

	printf("String built : %s\n\n", json_str->cstring);

	json_node* jnodep = parse_json(json_str);

	printf("Parsing done\n\n");
	
	print_json_node(jnodep);

	printf("Printing done\n\n");

	dstring* result = get_dstring("", 10);
	serialize_json(result, jnodep);
	printf("json string : ");
	display_dstring(result);
	printf("\n");
	delete_dstring(result);

	printf("serializing done\n\n");

	if(jnodep != NULL)
	{
		delete_json_node(jnodep);

		printf("Json node deleted\n\n");
	}

	delete_dstring(json_str);

	printf("deletion of dstring\n\n");
	return 0;
}