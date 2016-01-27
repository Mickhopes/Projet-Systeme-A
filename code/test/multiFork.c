#include "syscall.h"

int main(){

	int i = 0;
	for (i = 0; i < 12; i++){
		ForkExec("multiThreads");
	}

	for (i = 0; i < 12; i++){
		Wait();
	}

	PutString("\nJe suis le main, et tous mes fils sont terminés\n");

	return 0;
}