#include "syscall.h"
#define NULL 0

void print(){
	
	PutChar('Q');
	UserThreadExit();
	
}	


int main(){
	
	if(UserThreadCreate(print, NULL) == -1)
		PutString("Erreur lors de la création du thread\n");
		
	PutChar('z');
	return 0;
}
