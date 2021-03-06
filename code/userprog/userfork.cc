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
		//printf("no filename");
		currentThread->errorno = EINEX;
		return -1;
	}

	//+1 car exclure le main de la limite de 12.
	if (nbProc == MaxForkExec+1) {
		currentThread->errorno = EMAXPROC;
		return -1;
	}

    OpenFile *executable = fileSystem->Open (filename);
    AddrSpace *space;

    if (executable == NULL)
    {
	  //printf ("Unable to open file %s\n", filename);
	  currentThread->errorno = EINEX;
      currentThread->space->semWait->V();
	  return -1;
    }
    space = new AddrSpace (executable);

    int pid = Thread::FindProcId();

    /*char name[12];
    sprintf(name, "processus %d", pid);
    name[11] = '\0';*/

    Thread *newThread = new Thread("processus", -1, pid, currentThread->pid);
    
    newThread->space = space;
    newThread->space->semWaitFromFather = currentThread->space->semWait;

    delete executable;		// close file

    //Increase number of process running. (We need it for exit)
    semNumProc->P();
    nbProc++;
    semNumProc->V();

    newThread->ForkExec(StartForkExec, 0);

    return newThread->pid;
}

int do_Wait(){
  DEBUG ('z', "Wait par process %s\n",currentThread->pid);

  currentThread->space->semWait->P();

  return 0;
}