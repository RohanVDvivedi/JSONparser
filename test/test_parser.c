#include<stdio.h>
#include<unistd.h>

#include<json_parser.h>
#include<json_serializer.h>

#include<file_descriptor_stream.h>

int main()
{
	stream std_read;
	stream std_write;
	initialize_stream_for_fd(&std_read, STDIN_FILENO);
	initialize_stream_for_fd(&std_write, STDOUT_FILENO);

	int error = 0;
	json_node* js = parse_json(&std_read, 1024, 128, &error);

	if(!error)
	{
		serialize_json(&std_write, js);
		int error = 0;
		flush_all_from_stream(&std_write, &error);
		delete_json_node(js);
	}
	else
		printf("parsing error %d\n", error);

	deinitialize_stream(&std_read);
	deinitialize_stream(&std_write);
	return 0;
}