#include<json_parser.h>

int main()
{
	json_node* jnodep = parse_json(get_dstring(""));
	print_json_node(jnodep);
	delete_jnode(jnodep);
	return 0;
}