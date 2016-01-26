#ifndef ERRORNO_H
#define ERRORNO_H

#define EDEADLK 1		// A deadlock has been detected
#define EINVAL 2		// A thread wanted to join a thread already waited
#define ESRCH 3			// The wanted thread to be joined does not exist
#define ESPACE 4		// Not enough space left for user thread creation 
#define EMAXTHREADS 5	// Too much thread running at a time
#define EINEX 6			// The executable file to be executed by a process does not exist
#define EMAXPROC 7		// Too much process running at a time
#define EMAXSEM 8		// Too much semaphore
#define ESEMINVAL 9 	// Semaphore not found

#endif /* ERRORNO_H */