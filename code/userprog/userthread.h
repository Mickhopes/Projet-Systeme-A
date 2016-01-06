#ifndef USERTHREAD_H
#define USERTHREAD_H

#include "utility.h"

struct ThreadArgs {
	VoidFunctionPtr func; 
	int arg;
};

/* Create a user thread */
extern int do_UserThreadCreate(int f, int arg);

/* End a user thread */


#endif /* USERTHREAD_H */
