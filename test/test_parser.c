#include<stdio.h>
#include<unistd.h>

#include<json_parser.h>

int main()
{
	stream std_read;
	stream std_write;
	initialize_stream_for_fd(&std_read, STDIN_FILENO);
	initialize_stream_for_fd(&std_write, STDOUT_FILENO);

	json_node* js = parse_json(&std_read, 1024, 64);

	if(js != NULL)
		delete_json_node(js);

	deinitialize_stream(&std_read);
	deinitialize_stream(&std_write);
	return 0;
}