#include<json_parser.h>

int main()
{
	json_node* jnodep = parse_json(get_dstring("", 10));
	print_json_node(jnodep);
	if(jnodep != NULL)
	{
		delete_json_node(jnodep);;
	}
	return 0;
}