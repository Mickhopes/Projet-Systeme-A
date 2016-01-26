#ifndef USERFORK_H
#define USERFORK_H

extern int do_ForkExec (char *filename);

/* Wait for the first child to die */
extern int do_Wait();

#endif /* USERFORK_H */