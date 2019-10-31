INC_DIR=./inc
OBJ_DIR=./obj
LIB_DIR=./lib
SRC_DIR=./src
BIN_DIR=./bin

CC=gcc
RM=rm -f

TARGET=libjsonpar.a


CFLAGS=-I${INC_DIR}

${OBJ_DIR}/%.o : ${SRC_DIR}/%.c ${INC_DIR}/%.h
	${CC} ${CFLAGS} -c $< -o $@

${BIN_DIR}/$(TARGET) : ${OBJ_DIR}/jsonparser.c
	ar rcs $@ ${OBJ_DIR}/*.o

path : 
	@echo "export JSON_PARSER_PATH=\`pwd\`"

all : ${BIN_DIR}/$(TARGET)

clean :
	$(RM) $(BIN_DIR)/* $(OBJ_DIR)/*