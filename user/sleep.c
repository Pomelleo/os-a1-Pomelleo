//sleep.c
//Takes the arguements sleep <ticks> to pause for amount of ticks (1/10 seconds)

#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
	if(argc != 2 || atoi(argv[1]) == 0) //checks for enough arguements or valid digits
	{
	fprintf(2, "usage: sleep <ticks>");
	exit(1);
	}
	
	pause(atoi(argv[1])); //pause for (argv[1] to int conversion) 
	exit(0);
}

//example
//cat README notes 
//argc = 3
//argv[0] = cat
//argv[1] = README
//argv[2] = notes
