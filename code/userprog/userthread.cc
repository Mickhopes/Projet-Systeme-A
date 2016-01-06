int do_UserThreadCreate(int f, int arg) {
	Thread *newThread = new Thread("user");

	newThread->space = currentThread->space;
	//Permet de ne pas copier l'espace d'adressage un autre processus en cours

	struct ThreadArgs *args = new ThreadArgs;
	args->func = (VoidFunctionPtr) f;
	args->arg = arg;

	newThread->Fork(StartUserThread, (int)args);
}

static void StartUserThread(int f) {
	currentThread->space->InitRegister();
	currentThread->space->RestoreState();

	// Recupérer l'adresse du pointeur de pile
	// avec la fonction dans addrspace.cc
	// Si != -1, on remplit les registres et on fait Machine::Run()
	// Sinon currentThread->Finish();

	delete (struct ThreadArgs*)f;
	machine->Run();
}