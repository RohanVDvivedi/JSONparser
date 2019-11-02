#include<json_parser.h>

int main()
{
	printf("json string building start\n\n");

	dstring* json_str = get_dstring("\
	{ \
		\"name\" : \"Rohan\" \
		\"age\" : 23 \
		\"smart\" : true \
		\"dependents\" : null \
	} \
	", 10);

	printf("String built : %s\n\n", json_str->cstring);

	json_node* jnodep = parse_json(json_str);

	printf("Parsing done\n\n");
	
	print_json_node(jnodep);

	printf("Printing done\n\n");

	if(jnodep != NULL)
	{
		delete_json_node(jnodep);

		printf("Json node deleted\n\n");
	}

	delete_dstring(json_str);

	printf("deletion of dstring\n\n");
	return 0;
}