#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include<json_serializer.h>

#include<stream_util.h>
#include<stream_for_file_descriptor.h>

#define dst(x) get_dstring_pointing_to_literal_cstring(x)

int main()
{
	stream std_read;
	stream std_write;
	initialize_stream_for_fd(&std_read, STDIN_FILENO);
	initialize_stream_for_fd(&std_write, STDOUT_FILENO);

	json_node* js = new_json_object_node(10, (const json_object_entry[]){
		{dst("name"), new_json_string_node(&dst("Rohan"))},
		{dst("age"), new_json_unsigned_integer_node(26)},
		{dst("smart"), new_json_bool_node(0)},
		{dst("dependents"), NULL},
		{dst("family members"),
			new_json_array_node(2, (const json_node* []){
				new_json_array_node(4, (const json_node* []){
					new_json_integer_node(1),
					new_json_integer_node(2),
					new_json_integer_node(3),
					new_json_integer_node(4),
				}),
				new_json_array_node(4, (const json_node* []){
					new_json_string_node(&dst("1")),
					new_json_string_node(&dst("2")),
					new_json_string_node(&dst("3")),
					new_json_string_node(&dst("4")),
				})
			})
		},
		{dst("nick \t name"), new_json_string_node(&dst("Rohu"))},
		{dst("age \t next \n year"), new_json_integer_node(27)},
		{dst("\"IQ\""), new_json_float_node(196.05)},
		{dst("coder"), new_json_bool_node(1)},
		{dst("is dependent"), new_json_bool_node(1)},
	});

	int serialization_error;
	int stream_error = 0;

	serialization_error = serialize_json(&std_write, js);
	if(serialization_error)
	{
		printf("failed to serialize json\n");
		return 0;
	}
	flush_all_from_stream(&std_write, &stream_error);
	if(stream_error)
	{
		printf("failed to flush\n");
		return 0;
	}

	printf("\n\n");

	json_node* js_clone = clone_json_node(js);

	serialization_error = serialize_json(&std_write, js_clone);
	if(serialization_error)
	{
		printf("failed to serialize json\n");
		return 0;
	}
	flush_all_from_stream(&std_write, &stream_error);
	if(stream_error)
	{
		printf("failed to flush\n");
		return 0;
	}

	printf("\n\n");

	// remove an element from an array and object in js
	{
		delete_from_json_object(js, &dst("age \t next \n year"));

		json_node* family_members_array = fetch_json_node_from(js, &dst("family members"));
		json_node* family_members_array_1 = fetch_json_node_from(family_members_array, 1);
		delete_from_json_array(family_members_array_1, 2);
	}

	serialization_error = serialize_json(&std_write, js);
	if(serialization_error)
	{
		printf("failed to serialize json\n");
		return 0;
	}
	flush_all_from_stream(&std_write, &stream_error);
	if(stream_error)
	{
		printf("failed to flush\n");
		return 0;
	}

	printf("\n\n");

	serialization_error = serialize_json(&std_write, js_clone);
	if(serialization_error)
	{
		printf("failed to serialize json\n");
		return 0;
	}
	flush_all_from_stream(&std_write, &stream_error);
	if(stream_error)
	{
		printf("failed to flush\n");
		return 0;
	}

	printf("\n\n");

	delete_json_node(js);
	delete_json_node(js_clone);

	deinitialize_stream(&std_read);
	deinitialize_stream(&std_write);

	return 0;
}