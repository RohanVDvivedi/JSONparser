gcc ./test.c -I$JSON_PARSER_PATH/inc -I$CUTLERY_PATH/inc -L$JSON_PARSER_PATH/bin -L$CUTLERY_PATH/bin -ljsonpar -lcutlery
valgrind -v ./a.out