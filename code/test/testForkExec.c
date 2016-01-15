#include "syscall.h"

int main(){

	PutString("Création premier processus : ");
	PutInt(ForkExec("glacier"));
	PutChar('\n');

	/*PutString("Création deuxième processus : ");
	PutInt(ForkExec("userpages0"));
	PutChar('\n');

	int i = 0;
	int sum = 0;
	for(i = 0; i < 10000; i++) {
		sum += i/2;
	}*/

	return 0;
}