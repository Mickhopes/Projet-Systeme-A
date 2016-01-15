#include "syscall.h"

int main(){

	PutString("Création premier processus : ");
	PutInt(ForkExec("userpages0"));
	PutChar('\n');

	PutString("Création deuxième processus : ");
	PutInt(ForkExec("userpages1"));
	PutChar('\n');

	return 0;
}