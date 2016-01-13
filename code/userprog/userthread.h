#ifndef USERTHREAD_H
#define USERTHREAD_H

#include "utility.h"

struct ThreadArgs {
	VoidFunctionPtr func; 
	int arg;
	int stackAddr;
};

/* Create a user thread */
extern int do_UserThreadCreate(int f, int arg);

/* End a user thread */
extern void do_UserThreadExit();

/* Wait for the user thread threadId to finish */
extern int do_UserThreadJoin(unsigned int threadId);

/* Create a new process with the ForkExec syscall */
extern int do_NewProcess (char *filename);

#endif /* USERTHREAD_H */
