#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include<json_serializer.h>

#include<stream_util.h>
#include<file_descriptor_stream.h>

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
		{dst("nick name"), new_json_string_node(&dst("Rohu"))},
		{dst("age next year"), new_json_integer_node(27)},
		{dst("IQ"), new_json_float_node(196.05)},
		{dst("coder"), new_json_bool_node(1)},
		{dst("is dependent"), new_json_bool_node(1)},
	});

	int error = 0;

	serialize_json(&std_write, js);
	flush_all_from_stream(&std_write, &error);

	printf("\n\n");

	json_node* js_clone = clone_json_node(js);

	serialize_json(&std_write, js_clone);
	flush_all_from_stream(&std_write, &error);

	printf("\n\n");

	// remove an element from an array and object in js
	{
		json_object_entry* age_next_year = (json_object_entry*) find_equals_in_hashmap(&(js->json_object), &((const json_object_entry){.key = dst("age next year")}));
		delete_from_json_object(js, age_next_year);
	}

	serialize_json(&std_write, js);
	flush_all_from_stream(&std_write, &error);

	printf("\n\n");

	serialize_json(&std_write, js_clone);
	flush_all_from_stream(&std_write, &error);

	printf("\n\n");

	delete_json_node(js);
	delete_json_node(js_clone);

	deinitialize_stream(&std_read);
	deinitialize_stream(&std_write);

	return 0;
}