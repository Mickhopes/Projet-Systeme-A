#include "syscall.h"

int nbThreads = 30;
void print(void* arg) {
	int i;
	for (i = 0; i < 81; i++){
		PutChar('.');
	}
	UserThreadExit();
}	


int main(){

	int i;
	for (i = 1; i < nbThreads; i++){
		if(UserThreadCreate(print, 0) != -1){
			PutInt(i);
			PutChar('\n');
		}else{
			PutString("Err de creation de thread : \n\0");
			PutInt(i);
		}
	}

	for (i = 1; i < nbThreads; i++){
		UserThreadJoin(i);
	}

	PutString("Tous termine.\n\0");

	return 0;
}
