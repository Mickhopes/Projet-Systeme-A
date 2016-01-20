#ifndef ERRORNO_H
#define ERRORNO_H

#define EDEADLK 1		// A deadlock has been detected between two threads
#define EINVAL 2		// A thread wanted to join a thread already waited
#define ESRCH 3			// The wanted thread to be joined does not exist
#define ESPACE 4		// Not enough space left for user thread creation 
#define EMAXTHREADS 5	// Too much thread running at a time
#define EINEX 6			// The executable file to be executed by a process does not exist


/*-----------------------------------------------------------------
				Error on the filesys
-----------------------------------------------------------------*/
#define ENAMETOOLONG 7	//the name of the file is too long

// Global Variables
extern unsigned int errorno;

#endif /* ERRORNO_H */
