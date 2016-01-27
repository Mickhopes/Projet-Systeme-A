#include "syscall.h"

void calculs(void *args){
	PutString("Bonjour, je suis le thread : ");
	PutInt(GetTid());
	PutString(" du processus : ");
	PutInt(GetPid());
	PutChar('\n');

	char* var = "Je fais des calculs pour ne rien faire";
	char var2[39];
	int i = 0;
	for (i = 0; i < 39; i++){
		var2[i]= var[i];
	}
}


int main(){
	int i;

	for (i = 0; i < 12; i++){
		UserThreadCreate(calculs,(void*)0);		
	}

	return 0;
}