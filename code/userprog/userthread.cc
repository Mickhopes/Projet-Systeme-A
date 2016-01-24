#include "userthread.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"
#include "thread.h"
#include "addrspace.h"
#include "errorno.h"

/* We initialize and set the registers to call the function and then we run the machine */
static void StartUserThread(int arg) {
	// Initialization of registers
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();

	// We're setting the registers for the called function
	machine->WriteRegister(StackReg,(int)((ThreadArgs*)arg)->stackAddr); // Stack pointer Address
	machine->WriteRegister(PCReg, (int)((ThreadArgs*)arg)->func); // Address of the called function
    machine->WriteRegister(NextPCReg, (int)(((ThreadArgs*)arg)->func)+4); // Address of the next instruction of the called function
    machine->WriteRegister(RetAddrReg, (int)((ThreadArgs*)arg)->funcReturn); // Address to the function to return to. UserThreadExit in our case
    machine->WriteRegister(4, ((ThreadArgs*)arg)->arg); // Argument of the called function

    DEBUG ('z', "%d : StackReg [%d], PCReg [%d], NextPCReg [%d], Reg4 [%d]\n",currentThread->tid,(int)((ThreadArgs*)arg)->stackAddr,(int)((ThreadArgs*)arg)->func,(int)(((ThreadArgs*)arg)->func)+4,((ThreadArgs*)arg)->arg);
	delete (struct ThreadArgs*)arg;
	machine->Run();
}

int do_UserThreadCreate(int f, int arg, int fReturn) {
	// We find the next id available
	int tid = Thread::FindThreadId();

	/*char name[14];
	sprintf(name, "user thread %d", tid);
	name[13] = '\0';*/

	Thread *newThread = new Thread("user thread", tid, -1, currentThread->pid);

	newThread->space = currentThread->space;

	int address = currentThread->space->FindUserThreadSpace(&(newThread->idSpace), newThread->tid);
	if (address < 0){
		//Allocation of a new thread is not possible
		DEBUG ('z', "Creation thread impossible\n");
		if (address == -1) {
			// Because there is too much threads
			errorno = EMAXTHREADS;
		} else {
			// Because there is not enough space left
			errorno = ESPACE;
		}
		return -1;
	}

	if (currentThread->space->GetNbUserThreads() == 1) {
		DEBUG ('z', "Attente semaphore threadUser\n");
		currentThread->space->semWaitUserThreads->P();
		DEBUG ('z', "semaphore threadUser prit\n");
	}

	struct ThreadArgs *args = new ThreadArgs;
	args->func = (VoidFunctionPtr) f;
	args->funcReturn = (VoidFunctionPtr) fReturn;
	args->arg = arg;
	args->stackAddr = address;

	DEBUG ('z', "Creation de %s\n",newThread->getName());
	newThread->Fork(StartUserThread, (int)args);

	return newThread->tid;
}


void do_UserThreadExit(){
	DEBUG ('z', "Suppression de %s\n",currentThread->getName());

	Semaphore *sem = currentThread->space->RemoveUserThread(currentThread->idSpace);

	if (currentThread->space->GetNbUserThreads() == 0) {
		DEBUG ('z', "Liberation semaphore threadUser\n");
		currentThread->space->semWaitUserThreads->V();
	}
	
	if (sem != NULL) {
		sem->V();
	}

	currentThread->Finish();
}

int do_UserThreadJoin(unsigned int threadId){
	DEBUG ('z', "Join du thread %s sur %d\n",currentThread->getName(), threadId);

	currentThread->semJoin = new Semaphore("Join", 0);

	int ret;
	if ((ret = currentThread->space->IsJoinableUserThread(threadId, currentThread->tid)) < 0) {
		switch(ret) {
			case -1:
				errorno = ESRCH;
				break;
			case -2:
				errorno = ENVAL;
				break;
			case -3:
				errorno = EDEADLCK;
				break;
		}
		return -1;
	}

	currentThread->space->WaitForThread(threadId, currentThread->tid, currentThread->semJoin);

	return 0;
}
