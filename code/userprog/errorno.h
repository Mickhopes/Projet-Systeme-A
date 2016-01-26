#ifndef ERRORNO_H
#define ERRORNO_H

#define EDEADLK 1		// A deadlock has been detected between two threads
#define EINVAL 2		// A thread wanted to join a thread already waited
#define ESRCH 3			// The wanted thread to be joined does not exist
#define ESPACE 4		// Not enough space left for user thread creation 
#define EMAXTHREADS 5	// Too much thread running at a time
#define EINEX 6			// The executable file to be executed by a process does not exist
#define EMAXPROC 7		// Too much process running at a time
#define EMAXSEM 8		// Too much semaphore
#define ESEMINVAL 9 	// Semaphore not found
#define ENMETOOLONG 10	//the name of the file or directory is too long
#define ENAMEEXIST 11	//file or directory exist in current directory
#define EWTYPE 12		//the type of target is wrong(directory/File...)
#define ENAMENOTEXIST 13	//directory or file does not exist in current directory
#define EDIRNOTEMPTY 14	//directory isn't empty
#define EDIRFULL	15	//directory is full 
#define EHARDDISKFULL 16	//this error is raise if a hard disk havan't enough free blocks
#define EPATHNOTFIND 17	// the path name isn't exist

#endif /* ERRORNO_H */
