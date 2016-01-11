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

#endif /* USERTHREAD_H */
