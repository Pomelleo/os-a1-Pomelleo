//find.c
//#include "kernel/types.h"
//#include "kernel/fcntl.h"
//#include "user/user.h"

//int main (int argc, char *argv[])
//{
//	char *buffer[100];
//	int dirFd = open(argv[1], O_RDONLY);
//	int n = 1;
//	while(n != 0)				//while in file 
//	{
//	char *pattern = argv[2];
//	char bufferHist[100];
//	int match = 0;
//	int i = 0;
//	while(*pattern != '\0'){		//not end of search
//		while (*pattern == '*'){ pattern++;} //skip *
//		if(*pattern != '0'){	
//		read(dirFd, buffer, n);		//get buffer char
//		bufferHist[i] = *buffer[0];		//write buffer down
//		i++;
//		if (*buffer[0] == *pattern) {pattern++; match = 1;}
//		}		//if match check next char to look for
//		}
//	if (match == 1) fprintf(2, bufferHist); 
//	}
	


//	if(open(argv[1], O_RDONLY) < 0) 	//<dir> not found
//	{
//		fprintf(2, "find: cannot open <dir>");
//		exit(1);
//	}
//	fprintf(2,"Usage: find <dir> <pattern> [-type f|d]");

//	exit(1);
//}

//open returns fd > 0 if file found 
//open returns fd < 0 if file not found
//read(fd, buf(char array), how many chars to read)
//read reads directories as files 
//1 byte = 1 char 
//
//find.c
#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

int match(char *pattern, char *text) {
    if (*pattern == '\0') return 1;
    if (*pattern == '*') {
        pattern++;
        for (; *text != '\0'; text++) {
            if (match(pattern, text)) return 1;
        }
        return (*pattern == '\0');
    }
    if (*text == '\0') return 0;
    if (*pattern != *text) return 0;
    return match(pattern + 1, text + 1);
}

int main(int argc, char *argv[]) {
    char buf[16];        // Raw 16-byte directory entry
    int fd, n;
    char *pattern;
    int i, j, k;
    char name[14];       // Store filename
    
    //if(open(argv[1], O_RDONLY) < 0) 	//<dir> not found - ORIGINAL COMMENT KEPT
    if(argc < 3) {
        fprintf(2, "Usage: find <dir> <pattern>\n");
        exit(1);
    }
    
    pattern = argv[2];
    
    // Open the directory
    fd = open(argv[1], O_RDONLY);
    if(fd < 0) {
        fprintf(2, "find: cannot find %s\n", argv[1]);
        exit(1);
    }
    
    //while(n != 0)				//while in file - ORIGINAL COMMENT KEPT
    while((n = read(fd, buf, 16)) == 16) {
        // Check if empty entry (first 2 bytes are inode - both zero means empty)
        if(buf[0] == 0 && buf[1] == 0) continue;
        
        // Extract name from bytes 2-15 (skip 2-byte inum)
        for(i = 0; i < 14; i++) {
            name[i] = buf[2 + i];
            if(name[i] == '\0') break;  // Stop at null terminator
        }
        name[i] = '\0';  // Ensure null termination
			 //
        // Check if matches pattern
        if(!match(pattern, name)) continue;
        
        // PRINT FULL FILE PATH - dirname/filename
        int dirLen = strlen(argv[1]);
        int nameLen = strlen(name);
        
        // Write directory path
        for(j = 0; j < dirLen; j++) {
            write(1, &argv[1][j], 1);
        }
        write(1, "/", 1);
        // Write filename
        for(k = 0; k < nameLen; k++) {
            write(1, &name[k], 1);
        }
        write(1, "\n", 1);
    }
    
    close(fd);
    exit(0);
}
