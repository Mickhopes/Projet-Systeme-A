//Include car do_UserThreadCreate est extern.
#include "userthread.h"
#include "system.h"
#include "syscall.h"
#include "machine.h"
#include "thread.h"
#include "addrspace.h"

//TODO : trad les commentaire en anglais ????

/*Recupére l'adresse du pointeur de pile
	* grace a FindUserThreadSpace.
	* Si address == -1, currentThread->Finish(); 
	* Sinon on initialise puis remplit les registres et on fait Machine::Run() */
static void StartUserThread(int f) {
	//Initialisation des registres.
	currentThread->space->InitRegisters();
	currentThread->space->RestoreState();
	//Ecritures dans les registres pour preparer le lancement du thread.
	machine->WriteRegister(StackReg,(int)((ThreadArgs*)f)->stackAddr);//Adresse du pointeur de pile pour le nouveau thread.
	machine->WriteRegister(PCReg, (int)((ThreadArgs*)f)->func);
    machine->WriteRegister(NextPCReg, (int)(((ThreadArgs*)f)->func)+4);
    machine->WriteRegister(4, ((ThreadArgs*)f)->arg);
    //Place 0 dans le registre 2. SUCCESS.
    machine->WriteRegister(2,0);

	delete (struct ThreadArgs*)f;
	machine->Run();
}

int do_UserThreadCreate(int f, int arg) {
	Thread *newThread = new Thread("user");

	newThread->space = currentThread->space;
	//Permet de ne pas copier l'espace d'adressage un autre processus en cours

	int address = currentThread->space->FindUserThreadSpace(&(currentThread->id));
	if (address == -1){
		//Allocation of a new thread is not possible
		return -1;
	}

	if (currentThread->space->GetNbUserThreads() == 1) {
		semWaitUserThreads->P();
	}

	struct ThreadArgs *args = new ThreadArgs;
	args->func = (VoidFunctionPtr) f;
	args->arg = arg;
	args->stackAddr = address;

	newThread->Fork(StartUserThread, (int)args);

	return 0;
}


void do_UserThreadExit(){
	//Il faut que les User thread appele UserThreadExit avant que le main se termine. CF. userprog/exception.cc AS:SC_Exit(ligne 166)
	currentThread->space->RemoveUserThread(currentThread->id);

	if (currentThread->space->GetNbUserThreads() == 0) {
		semWaitUserThreads->V();
	}

	currentThread->Finish();
}