gcc ./test_serializer.c -o test_serializer.out -ljsonparser -lcutlery
gcc ./test_parser.c -o test_parser.out -ljsonparser -lcutlery

# standard test case for parsing aswell
#printf "{\"age\":26,\"nick \t name\":\"Rohu\",\"coder\":true,\"dependents\":null,\"is dependent\":true,\"age \t next \n year\":27,\"IQ\":196.050000,\"name\":\"Rohan\",\"family members\":[[1,2,3,4],[\"1\",\"2\",\"3\",\"4\"]],\"smart\":false}" | ./test_parser.out