#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum inside {assign=0,block=1,array=2,nothing=100};
typedef enum inside inside;inside null = nothing;

typedef struct node node;
struct node
{
	char* data;				// string of the node property
	int valuescount;		// the number of values
	int totalspace;			// total nodes it can store
	node** values;			// values
	int error;				// in case acquiring that property has been successfull if 1 cconfig file is damaged
};

struct stack
{
	int numofelements;
	inside** array;
	int arraysize;
};
typedef struct stack stack;

void getextension(char* filename,char* extension);

node* constructfiletree(char* filename);					// call this function and pass in the filename
void deletefiletree(node* filetreehead,char* filename);		// free data after your work with the corresponding tree has been completed
void printfiletree(node* filetreenode,const char* printable);		// print JSON file as the structure that this tree is stored in this program

int main(int argc,char** argv)	// just a driver function
{
	char* filename;
	char extension[10];
	
	switch(argc)		// take only one filename as input
	{
		case 1:
			{
				printf("too few arguments\n");
				return -1;
			}
		case 2:
			{
				filename = argv[1];
				getextension(filename,extension);
				break;
			}
		default :
			{
				printf("too many arguments\n");
				return -1;
			}
	}
	
	
	node* filetreehead = constructfiletree(filename);	// ask to read JSON file and give you the structure
	
	// do what ever you want to do with the tree
	printfiletree(filetreehead,NULL);
	// so we are stupid enough to only print it
	
	deletefiletree(filetreehead,filename);				// delete file tree that was generated
	
	return 0;
}


void getextension(char* filename,char* extension)
{
	int i=0,j=0;
	while( filename[i]!='\0' && filename[i++]!='.'){}
	for(;filename[i+j]!='\0' && j<9;j++)
	{
		extension[j] = filename[i+j];
	}
	extension[j] = '\0';
}

//
//	THIS  BLOCK CONTRIBUTES TO THE MAIN STACK OF THE CONTENT HOLDER OF ENUM inside
//

void initstack(stack* s)
{
	s->numofelements = 0;
	s->arraysize=0;
	s->array=NULL;
}

void pushonstack(stack* s,inside* i)	// method to push a data entry on stack
{
	if(s->arraysize == s->numofelements)
	{
		int newarraysize = s->arraysize + 10;
		inside** newarray = ( (inside**) (malloc( sizeof(inside*)*newarraysize )) );
		for(int i=0;i<s->numofelements;i++)
		{
			newarray[i] = s->array[i];
		}
		free(s->array);
		s->array = newarray;
		s->arraysize = newarraysize;
	}
	// below are main two lines that work for putting a content holder on the stack
	s->array[s->numofelements] = ( (inside*) malloc( sizeof(inside) ) );
	*(s->array[s->numofelements++]) = *i;
	// as seen the element on the stack has its own data allocated, and pointed to by the last element in the array "enum inside" pointers
	// then value is copied from old pointer to the last element pointer
	//  note we copy data and not the pointer, hence i is just to get access to data pointed to by i
	// long story short :: stack handles its data on its own
}

void popfromstack(stack* s)
{
	if(s->numofelements == 0)
	{
		return;
	}
	free( s->array[--(s->numofelements)] );
	// just one line to free last data and decrement the number of elements
}

inside* checklast(stack* s)	// get pointer tot the last element of the stack
{
	if(s->numofelements == 0)	// if stack size is zero return NULL
	{
		return &null;
	}
	return s->array[s->numofelements-1];
}

void deletestack(stack* s)	// delete stack when the tree construction is completed
{
	int tempnumofelements = s->numofelements;
	for(int i=0;i<s->numofelements;i++)
	{
		free(s->array[i]);
	}
	s->numofelements = 0;
	s->arraysize=0;
	if(tempnumofelements!=0)
	{
		free(s->array);
	}
}

void printstack(stack* s)	// printing stack for debug purposes
{
	for(int i=0;i<s->numofelements;i++)
	{
		printf("%d ",((int)(*(s->array[i]))) );
	}
	printf("\n");
}

//
//	THIS IS AN END OF THE STACK IMPLEMENTATION FOR STORING THE CONTENT HOLDER OF THE FILE NAMINGLY "assign","block","array" all of type enum inside
//


char* getstringuntil(FILE* filepointer,char end,int* line) // this function helps to get a string from file until a end character is read
{
	char c;
	char* data = ( (char*) ( malloc( sizeof(char)*100 ) ) );
	int datasize = 100;
	int stringsize = 0;
	int loopend = 0;
	while(loopend==0)
	{
		c = fgetc(filepointer);
		switch (c)
		{
			case EOF:
			{
				data[stringsize++] = '\0';
				loopend = 1;
				break;
			}
			case '\n':
			{
				(*line)++;
			}
			default :
			{
				if(c==end)
				{
					data[stringsize++] = '\0';
					loopend = 1;
					break;
				}
				if(stringsize==datasize)	// if string is full ask for some 50 more characters
				{
					int newdatasize = datasize+50;
					char* newdata = ( (char*) ( malloc( sizeof(char)*newdatasize ) ) );
					for(int i=0;i<stringsize;i++)
					{
						newdata[i] = data[i];
					}
					free(data);
					data = newdata;
					datasize = newdatasize;
				}
				data[stringsize++] = c;
				break;
			}
		}
	}
	return data;
}

char* getarraystringname(int number)	// this function provides a string name to an array element in the JSON file
{
	char* num = ( (char*) (malloc(sizeof(char)*12)) );
	int i = 0;
	if(number==0)
	{
		num[0] = '0';
		num[1] = '\0';
		return num;
	}
	while(number>0 && i<10)
	{
		num[i++] = number%10 + '0';
		number/=10; 
	}
	
	char temp;
	for(int j=0;j<i/2;j++)
	{
		temp=num[j];
		num[j]=num[i-j-1];
		num[i-j-1]=temp;
	}
	
	num[i] = '\0';
	return num;
}

void addnodeto(node* parent,node* child);
node* createnewnode(char* data);
int parsefileandgettree(node* filetreehead,FILE* filepointer,stack* s,int* line);

node* constructfiletree(char* filename)	// this is the caller function that makes the first filename node
{
	node* filetreehead = createnewnode(filename);	// create first node in the tree based on the file name
	
	FILE *filepointer = fopen(filename,"r");
	if(filepointer!=NULL)
	{
		int line = 1;
		filetreehead->error = 0;
		stack s;
		initstack(&s);		// intializes stack for the parser brackets
		parsefileandgettree(filetreehead,filepointer,&s,&line);// it then calls the main function that parses file and creates rest of the tree
		deletestack(&s);	// deletes the stack
							// stacks is empty if there has been no error
	}
	fclose(filepointer);
	return filetreehead;
}

int parsefileandgettree(node* parent,FILE* filepointer,stack* s,int* line)	// this the main function that creates the JSON file tree from the file
{
	char c;
	int brek = 1;
	int arrayiter = 0;
	inside temp;
	node* child;
	
	int update_c = 1;
	
	char* bare = NULL;
	
	int commasafe = 1;
	
	while(brek)
	{
		if(update_c==1)
		{
			c = fgetc(filepointer);
		}
		else
		{
			update_c =1;
		}
		
		
		switch(c)
		{
			case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
			{
				if( *checklast(s) == assign && commasafe == 0 )
				{
					printf("error separation comma has not been found :: line no. %d\n",(*line));
					return 11;
				}
				if( *checklast(s) != assign )
				{
					printf("error integral values only after ':' :: line no. %d\n",(*line));
					return 11;
				}
				// if that is the case grab the number
				int baresize = 12;
				int charsread = 0;
				bare = ( (char*) malloc( sizeof(char) * baresize ) );
				int decimaloccurence = 1;
				#define deocc decimaloccurence
				do
				{
					if(baresize==charsread)
					{
						baresize+= 15;
						char* newbare = ( (char*) (malloc(sizeof(char)*baresize)) );
						for(int i=0;i<charsread;i++)
						{
							newbare[i] = bare[i];
						}
						free(bare);
						bare = newbare;
					}
					bare[charsread++] = c;
				}while( ( ((c=fgetc(filepointer))=='x' || c=='o' || c=='b' || c=='d') && charsread==1 && bare[0]=='0' && (deocc--) ) || ( c=='.' && (deocc--) ) || (c>='0' && c<='9') );
				
				while( c==' ' || c=='\n' || c=='\t' )
				{
					if(c=='\n')
					{
						(*line)++;
					}
					c=fgetc(filepointer);
				}
				
				if(c==']' || c=='}' || c==',')
				{
					child = createnewnode( bare );
					addnodeto(parent,child);
					commasafe = 0;
				}
				else
				{
					while( c!=']' && c!='}' && c!=',' )
					{
						c=fgetc(filepointer);
						if(c=='\n')
						{
							(*line)++;
						}	
					}
					free(bare);
					printf("Error reading number :: line no. %d\n",(*line));
				}
				brek = 1;
				update_c = 0;
				break;
			}
			
			case '\"':
			{
				if( *checklast(s) == assign && commasafe == 0 )
				{
					printf("error separation comma has not been found :: line no. %d\n",(*line));
					return 11;
				}
				child = createnewnode( getstringuntil(filepointer,'\"',line) );
				addnodeto(parent,child);
				if( *checklast(s) == assign )
				{
					commasafe = 0;
				}
				brek = 1;
				break;
			}
			case ':':
			{
				if( s->numofelements!=0 && *checklast(s) == assign )
				{
					printf("improper location as a value of value :: line no. %d\n",(*line));
					return 11;
				}
				temp = assign;
				pushonstack(s,&temp);
				int test = parsefileandgettree(child,filepointer,s,line);
				switch( test )
				{
					case 14 :
					{
						switch( *checklast(s) )
						{
							case block:
							{
								return 24;
							}
							case array:
							{
								return 34;
							}
						}
						break;
					}
					case 54 :
					case 64 :
					{
						return test;
					}
					case 11 :
					{
						popfromstack(s);
						return 11;
					}
				}
				
				break;
			}
			case ',':
			{
				commasafe = 1;
				switch( *checklast(s) )
				{
					case assign:
					{
						if( parent->valuescount == 0)
						{
							printf("warning not enought values :: line no. %d\n",(*line));
						}
						popfromstack(s);
						return 14;
					}
					case block:
					{
						return 24;
					}
					case array:
					{
						return 34;
					}	
				}
			}
			case '{':
			{
				if(commasafe==0)
				{
					printf("braces must be separated by comma :: line no. %d\n",(*line));
					return 11;
				}
				int test;
				temp = block;
				pushonstack(s,&temp);
				int loopitered = 0;	// an iterator to keep track of the num of elements in this braces
				while( (test=parsefileandgettree(parent,filepointer,s,line))!=64 && test!=11){ loopitered++;}
				commasafe = 0;
				if(test==11)
				{
					return 11;
				}
				break;
			}
			case '[':
			{
				if(commasafe==0)
				{
					printf("braces must be separated by comma :: line no. %d\n",(*line));
					return 11;
				}
				int test;
				temp = array;
				pushonstack(s,&temp);
				int loopitered = -1;	// an iterator to keep track of the num of elements in this braces
				do
				{
					child = createnewnode( getarraystringname(arrayiter++) );
					addnodeto(parent,child);
					loopitered++;
				}
				while( (test=parsefileandgettree(child,filepointer,s,line))  != 54 && test!=11);
				commasafe = 0;
				if( loopitered == 0)	// if no child has been added then clear the only rubbish created
				{
					parent->valuescount--;
					free(child->data);
					free(child);
				}
				if(test==11)
				{
					return 11;
				}
				break;
			}
			case ']':
			{
				switch( *checklast(s) )
				{
					case assign :
					{
						if( parent->valuescount == 0)
						{
							printf("warning not enought values :: line no. %d\n",(*line));
						}
						popfromstack(s);
						// no break here, it is not a mistake
					}
					case array :
					{
						popfromstack(s);
						return 54;
					}
				}
				printf("couldn't end by ] there must be an error :: line no. %d\n",(*line));
				return 11;
			}
			case '}':
			{
				switch( *checklast(s) )
				{
					case assign :
					{
						if( parent->valuescount == 0)
						{
							printf("warning not enought values :: line no. %d\n",(*line));
						}
						popfromstack(s);
						// no break here, it is not a mistake
					}
					case block :
					{
						popfromstack(s);
						return 64;
					}
				}
				printf("couldn't end by } there is an error :: line no. %d\n",(*line));
				return 11;
			}
			case (EOF) :
			{
				return 11;
			}
			case '\n':
			{
				(*line)++;
			}
			default :
			{
				brek = 1;
				break;
			}
		}
		
	}
	return 11;
}

void addnodeto(node* parent,node* child)	// takes two pointers parent and child and make a parent value pinter point to child
{	
	if(parent->valuescount == parent->totalspace)	// in case that the total space is full we need new memory to store the new pointer
	{
		int newtotalspace = parent->totalspace + 5;		// for optimization we ask for 5 new node pointer locations in advance
		node** newvalues = ( (node**) ( malloc( sizeof(node*)*newtotalspace ) ) );
		for(int i=0;i<parent->totalspace;i++)
		{
			newvalues[i] = parent->values[i];
		}
		free(parent->values);
		parent->values = newvalues;
		parent->totalspace = newtotalspace;
	}
	parent->values[parent->valuescount++] = child;	// this is the main line of the function that makes a parent point to child
}

node* createnewnode(char* data)	// this method initializes all the memory required by node except its string data pointer
{
	node* newnode = ( (node*) (malloc(sizeof(node))) );	// memory allocation to the new data required
	newnode->data = data;								// as seen we donot hold any memory required for data here
	newnode->values = NULL;								// neither is its child value pointers allocated 
	newnode->valuescount = 0;
	newnode->totalspace = 0;
	newnode->error = 1;									// presents any error (error is made 0 if no error is found)
}

void deletefiletree(node* filetreenode,char* filename)		// recusively free all tree data
{
	for(int i=0;i<filetreenode->valuescount;i++)
	{
		deletefiletree(filetreenode->values[i],filename);	// delete childe memories first
	}
	if(filetreenode->data != filename )			// since the filename string is the same as the data in main node
	{
		free(filetreenode->data);				// the if satement prevents us from deleting the filename string
	}
	if(filetreenode->totalspace > 0)			// remove delete the node** array if it is actually occupying any space
	{
		free(filetreenode->values);
	}
	free(filetreenode);			// finally delete the node itself after the rest data of node is completely free
}

char* concat(const char *s1, const char *s2)	// String concatenation function
{
    const size_t len1 = s1==NULL?0:strlen(s1);
    const size_t len2 = s1==NULL?0:strlen(s2);
    
    char *result = malloc(len1+len2+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    memcpy(result, s1, len1);
    memcpy(result+len1, s2, len2);//+1 to copy the null-terminator
    result[len1+len2] = '\0';
    return result;
}

char pointsto[10] = " -> ";		// this global variable helps show a pointed arrow to show a child for printing
void printfiletree(node* filetreenode,const char* printable)
{
	char* printable_local;
	
	char* printable_temp = concat( printable , pointsto );
	printable_local = concat( printable_temp , filetreenode->data );
	free(printable_temp);
	
	if(filetreenode->valuescount == 0 )
	{ 
		printf("%s\n",printable_local);
	}
	
	for(int i=0;i<filetreenode->valuescount;i++)
	{
		printfiletree( filetreenode->values[i] , printable_local );
	}
	
	free(printable_local);
}
