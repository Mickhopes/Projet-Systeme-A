#ifndef ERRORNO_H
#define ERRORNO_H

#define EDEADLCK 1		// A deadlock has been detected between two threads
#define ENVAL 2		// A thread wanted to join a thread already waited
#define ESRCH 3			// The wanted thread to be joined does not exist
#define ESPACE 4		// Not enough space left for user thread creation 
#define EMAXTHREADS 5	// Too much thread running at a time
#define EINEX 6			// The executable file to be executed by a process does not exist


/*-----------------------------------------------------------------
				Error on the filesys
-----------------------------------------------------------------*/
#define ENMETOOLONG 7	//the name of the file or directory is too long
#define ENAMEEXIST 8	//file or directory exist in current directory
#define EWTYPE 9		//the type of target is wrong(directory/File...)
#define ENAMENOTEXIST 10	//directory or file does not exist in current directory
#define EDIRNOTEMPTY 11	//directory isn't empty
#define EDIRFULL	12	//directory is full 
#define EHARDDISKFULL 13	//this error is raise if a hard disk havan't enough free blocks




// Global Variables
extern unsigned int errorno;

#endif /* ERRORNO_H */
