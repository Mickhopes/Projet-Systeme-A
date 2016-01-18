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
/*WARNING if exit or quit is a filename the call thread is stop not correctly
* if background process if possible change this code is compulsory
*/
int
do_ForkExec (char *filename)
{
	if(filename == NULL)
	{
		printf("no filename");
		errorno = EINEX;
		return -1;
	}
	else if(strcmp(filename, "quit") == 0 || strcmp(filename, "exit") == 0)
	{
		//TODO change code if autorise background process
		DEBUG('w', "azertyuiopjhgfdfghjkjhgfdjkghkhqkjeshfkhefkhzemihfQP\n");
		//currentThread->Finish ();	
		interrupt->Halt();
		return 0;
	}
    OpenFile *executable = fileSystem->Open (filename);
    AddrSpace *space;

    if (executable == NULL)
    {
	  printf ("Unable to open file %s\n", filename);
	  errorno = EINEX;
	  return -1;
    }
    space = new AddrSpace (executable);

    int pid = Thread::FindProcId();

    /*char name[12];
    sprintf(name, "processus %d", pid);
    name[11] = '\0';*/

    Thread *newThread = new Thread("processus", -1, pid, currentThread->pid);
    
    newThread->space = space;

    delete executable;		// close file

    //Increase number of process running. (We need it for exit)
    semNumProc->P();
    nbProc++;
    semNumProc->V();

    newThread->ForkExec(StartForkExec, 0);

    return newThread->pid;
}
