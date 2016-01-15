#include "userthread.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"
#include "thread.h"
#include "addrspace.h"
#include "errorno.h"

static void StartForkExec(int arg) {
	// Initialization of registers
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();

	DEBUG('z', "Le processus %s, ppid: %d va se lancer\n", currentThread->getName(), currentThread->ppid);
	machine->Run();
}

int
do_ForkExec (char *filename)
{
    OpenFile *executable = fileSystem->Open (filename);
    AddrSpace *space;

    if (executable == NULL)
      {
	  printf ("Unable to open file %s\n", filename);
	  return -1;
      }
    space = new AddrSpace (executable);

    int pid = Thread::FindProcId();

    char name[12];
    sprintf(name, "processus %d", pid);
    name[11] = '\0';

    Thread *newThread = new Thread(name, -1, pid, currentThread->pid);
    
    newThread->space = space;

    delete executable;		// close file

    //Increase number of process running. (We need it for exit)
    semNumProc->P();
    nbProc++;
    semNumProc->V();

    newThread->ForkExec(StartForkExec, 0);

    return newThread->pid;
}