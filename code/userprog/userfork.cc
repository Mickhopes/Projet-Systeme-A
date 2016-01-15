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

	DEBUG('z', "Le processus %s, pid: %d, ppid: %d va se lancer\n", currentThread->getName(), currentThread->pid, currentThread->ppid);
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
    Thread *newThread = new Thread("Proc", -1, Thread::FindProcId(), currentThread->pid);
    
    newThread->space = space;

    delete executable;		// close file

    //Increase number of process running. (We need it for exit)
    semNumProc->P();
    nbProc++;
    semNumProc->V();

    newThread->ForkExec(StartForkExec, 0);

    return newThread->pid;
}