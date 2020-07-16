# JSONparser
A small project to implement a recursive top down parser, for a simple JSON grammar.

compile the project as : gcc JSONreader.c -o JSONread.out

run the project as     : ./JSONread.out <your JSON filename>

The project is in C source code, It forms a Parse tree , a multi node parse tree, for the given data. the parent node is always the file name.

basically when there is an array [] each of individual element in the array, has separate parents as strings that are formed of decimal numbers in ASCII. so "mainparent" : ["foam":"stud","nudge":"rude"]
here "mainparent" is the main parent that has two children "0" and "1". child of "0" is "foam", and child of foam is "stud".

for the block operator the parent remains same so "mainparent" : {"foam":"stud","nudge":"rude"}
here main parent has two child "foam" and "rude".

The tree can be further used to extract data from the file in its format. It does not support null,true and false.

setup instructions
 * git clone https://github.com/RohanVDvivedi/JSONparser.git
 * cd JSONparser
 * sudo make clean install
 * add "-ljsonpar" linker flag, while compiling your application