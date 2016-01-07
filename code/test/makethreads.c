#include "syscall.h"
#define NULL 0

void print(){
	
	PutString("Dans le thread\n");
	UserThreadExit();
	
}	


int main(){
	
	if(UserThreadCreate(print, NULL) == -1)
		PutString("Erreur lors de la création du thread\n");
		
	PutString("main\n");
	return 0;
}
