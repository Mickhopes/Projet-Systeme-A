// exception.cc 
//      Entry point into the Nachos kernel from user programs.
//      There are two kinds of things that can cause control to
//      transfer back to here from user code:
//
//      syscall -- The user code explicitly requests to call a procedure
//      in the Nachos kernel.  Right now, the only function we support is
//      "Halt".
//
//      exceptions -- The user code does something that the CPU can't handle.
//      For instance, accessing memory that doesn't exist, arithmetic errors,
//      etc.  
//
//      Interrupts (which can also cause control to transfer from user
//      code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
#include "userthread.h"
#include "userfork.h"
#include "synch.h"
#include "errorno.h"
#include "addrspace.h"
#include "userfile.h"

unsigned int errorno;

//----------------------------------------------------------------------
// UpdatePC : Increments the Program Counter register in order to resume
// the user program immediately after the "syscall" instruction.
//----------------------------------------------------------------------
static void
UpdatePC ()
{
    int pc = machine->ReadRegister (PCReg);
    machine->WriteRegister (PrevPCReg, pc);
    pc = machine->ReadRegister (NextPCReg);
    machine->WriteRegister (PCReg, pc);
    pc += 4;
    machine->WriteRegister (NextPCReg, pc);
}


//----------------------------------------------------------------------
// ExceptionHandler
//      Entry point into the Nachos kernel.  Called when a user program
//      is executing, and either does a syscall, or generates an addressing
//      or arithmetic exception.
//
//      For system calls, the following is the calling convention:
//
//      system call code -- r2
//              arg1 -- r4
//              arg2 -- r5
//              arg3 -- r6
//              arg4 -- r7
//
//      The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//      "which" is the kind of exception.  The list of possible exceptions 
//      are in machine.h.
//----------------------------------------------------------------------

/*void
ExceptionHandler (ExceptionType which)
{
    int type = machine->ReadRegister (2);

    if ((which == SyscallException) && (type == SC_Halt))
      {
	  DEBUG ('a', "Shutdown, initiated by user program.\n");
	  interrupt->Halt ();
      }
    else
      {
	  printf ("Unexpected user mode exception %d %d\n", which, type);
	  ASSERT (FALSE);
      }

    // LB: Do not forget to increment the pc before returning!
    UpdatePC ();
    // End of addition
}*/

void copyStringFromMachine(int from, char* to, unsigned int size) {
	unsigned int i = 0;
	for(i = 0; i < size; i++) {
		int c;
		machine->ReadMem(from+i, 1, &c);
		to[i] = (char) c;
	}

	to[size+1] = '\0';
}

void CopyStringInMachineMemory(char *linuxString, char* MipsString, unsigned int size) {
	unsigned int i = 0;
	while (linuxString[i] !='\0' && i < size) {
		machine->WriteMem((unsigned int)(MipsString+i), 1, linuxString[i]);
		i++;
	}
	machine->WriteMem((unsigned int)(MipsString+i), 1, '\0');
}

void
ExceptionHandler(ExceptionType which)
{
	int type = machine->ReadRegister(2);
	if (which == SyscallException) {
		switch (type) { // Type correspond to the type of system call
			case SC_Halt: {
				DEBUG('a', "Shutdown, initiated by user program.\n");

				//Prise du semaphore qui garantit que les User threads sont tous terminés.
				DEBUG ('z', "%s attend semaphore pour terminer\n",currentThread->getName());
				currentThread->space->semWaitUserThreads->P();
				DEBUG ('z', "%s a pris semaphore pour terminer\n",currentThread->getName());

				interrupt->Halt();
				break;
			}
			case SC_PutChar: {
				char c = (char)machine->ReadRegister(4); // We get the first argument
				synchconsole->SynchPutChar(c);
				break;
			}
			case SC_GetChar: {
				char c = (char)synchconsole->SynchGetChar();
				if (c == EOF) {
					DEBUG('a', "Shutdown, EOF in GetChar detected.\n");
					interrupt->Halt(); // If EOF, we stop the program
				} else {
					machine->WriteRegister(2, (int)c); // If not we write the return value in the second register
				}
				break;
			}
			case SC_PutString: {
				int mips_pointer = machine->ReadRegister(4); // We get the MIPS pointer to the string in argument
				char linux_pointer[MAX_STRING_SIZE];
				copyStringFromMachine(mips_pointer, linux_pointer, MAX_STRING_SIZE-1);
				synchconsole->SynchPutString(linux_pointer);
				break;
			}
			case SC_GetString: {
				char buff[MAX_STRING_SIZE];
				synchconsole->SynchGetString(buff, machine->ReadRegister(5));
				CopyStringInMachineMemory(buff,(char *)machine->ReadRegister(4), MAX_STRING_SIZE-1);
				
				break;
			}
			case SC_PutInt: {
				int n = machine->ReadRegister(4);
				synchconsole->SynchPutInt(n);
				break;
			}
			case SC_GetInt: {
				int n = synchconsole->SynchGetInt();
				machine->WriteRegister(2, n);
				break;
			}
			case SC_UserThreadCreate: {
				DEBUG ('z', "do_UserThreadCreate\n");
				machine->WriteRegister(2,do_UserThreadCreate(machine->ReadRegister(4), machine->ReadRegister(5), machine->ReadRegister(6)));
				break;
			}
			case SC_UserThreadExit: {
				DEBUG ('z', "do_UserThreadExit\n");
				do_UserThreadExit();
				break;
			}
			case SC_UserThreadJoin: {
				DEBUG ('z', "do_UserThreadJoin\n");
				machine->WriteRegister(2, do_UserThreadJoin(machine->ReadRegister(4)));
				break;
			}
			case SC_GetErrorNo: {
				machine->WriteRegister(2, errorno);
				break;
			}
			case SC_InitSemaphore: {
				Semaphore *sem = new Semaphore((char*)machine->ReadRegister(4), machine->ReadRegister(5));
				machine->WriteRegister(2, (int)sem);
				DEBUG ('s', "Creation semaphore de %d\n", machine->ReadRegister(5));
				break;
			}
			case SC_DestroySemaphore: {
				Semaphore *sem = (Semaphore*)machine->ReadRegister(4);
				DEBUG ('s', "Destruction semaphore\n");
				delete sem;
				break;
			}
			case SC_P: {
				Semaphore *sem = (Semaphore*)machine->ReadRegister(4);
				DEBUG ('s', "Avant prise semaphore\n");
				sem->P();
				DEBUG ('s', "Apres prise semaphore\n");
				break;
			}
			case SC_V: {
				Semaphore *sem = (Semaphore*)machine->ReadRegister(4);
				DEBUG ('s', "Avant relache semaphore\n");
				sem->V();
				DEBUG ('s', "Apres relache semaphore\n");
				break;
			}
			case SC_ForkExec: {
				DEBUG ('z', "do_ForkExec\n");
				int mips_pointer = machine->ReadRegister(4); // We get the MIPS pointer to the string in argument
				char linux_pointer[MAX_STRING_SIZE];
				copyStringFromMachine(mips_pointer, linux_pointer, MAX_STRING_SIZE-1);
				machine->WriteRegister(2,do_ForkExec(linux_pointer));
				break;
			}
			case SC_GetTid: {
				machine->WriteRegister(2, currentThread->tid);
				break;
			}
			case SC_GetPid: {
				machine->WriteRegister(2, currentThread->pid);
				break;
			}
			case SC_GetPPid: {
				machine->WriteRegister(2, currentThread->ppid);
				break;
			}
			case SC_Exit: {
				DEBUG('y', "Exit du processus %s\n", currentThread->getName());

				//Prise du semaphore qui garantit que les User threads sont tous terminés.
				DEBUG ('z', "%s attend semaphore pour terminer\n",currentThread->getName());
				currentThread->space->semWaitUserThreads->P();
				DEBUG ('z', "%s a pris semaphore pour terminer\n",currentThread->getName());

				//int ret = machine->ReadRegister(4);
				//printf("Return Value of main: %d\n", ret);

				DEBUG ('y', "Avant prise semaphore numProc sur le thread %s\n",currentThread->getName());
				semNumProc->P();
				DEBUG ('y', "Apres prise semaphore numProc sur le thread %s\n",currentThread->getName());
				if (nbProc == 1){
					DEBUG ('y', "Avant Halt sur le thread %s\n",currentThread->getName());
					semNumProc->V();
					interrupt->Halt();
				}else{
					nbProc--;
					DEBUG ('y', "%s décrémente nbProc, il reste %d processus\n",currentThread->getName(), nbProc);
					semNumProc->V();
					if (currentThread->space->semWaitFromFather != NULL){
						currentThread->space->semWaitFromFather->V();
					}
					delete currentThread->space;
					DEBUG ('y', "Avant finish sur le thread %s\n",currentThread->getName());
					currentThread->Finish();
				}
				break;
			}
			case SC_Waitpid: {
				DEBUG ('z', "do_Waitpid\n");
				machine->WriteRegister(2, do_Waitpid());
				break;
			}
			/*
			case SC_Create: {
				DEBUG ('z', "do_Create\n");
				machine->WriteRegister(2, do_Create((char*)machine->ReadRegister(4)));
				break;
			}
			
			case SC_Open: {
				DEBUG ('z', "do_Open\n");
				machine->WriteRegister(2, do_Open((char*)machine->ReadRegister(4)));
				break;
			}
			case SC_Read: {
				DEBUG ('z', "do_Read\n");
				machine->WriteRegister(2, do_Read((char*)machine->ReadRegister(4), machine->ReadRegister(5), machine->ReadRegister(6)));
				break;
			}
			case SC_Write: {
				DEBUG ('z', "do_Write\n");
				machine->WriteRegister(2, do_Write((char*)machine->ReadRegister(4), machine->ReadRegister(5), machine->ReadRegister(6));
				break;
			}
			case SC_Close: {
				DEBUG ('z', "do_Close\n");
				machine->WriteRegister(2, do_Close((char*)machine->ReadRegister(4)));
				break;
			}
			*/
			
			default: {
				printf("Unexpected user mode exception %d %d\n", which, type);
				ASSERT(FALSE);
			}
		}
		UpdatePC();
	}
}
