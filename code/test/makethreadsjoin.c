#include "syscall.h"
#define NULL 0


void attendre(void* arg) {
	
	PutString("J'attends le thread 1");
	//PutInt((int) arg);
	UserThreadJoin(1);
	
	UserThreadExit();
}	

void print(void *arg){

	PutString("coucou\n");
	//sleep
	int i = 0;
	while(i<20)
	{
		i++;
	}

	PutChar('\n');
	UserThreadExit();
}


int main(){
	
	if((UserThreadCreate(print, NULL)) != -1){
		//PutString("pas d'Erreur lors de la création du thread\n");
		//PutInt(id);
		PutString("Je suis le premier !\n");
	}
	if(UserThreadCreate(attendre,NULL) != -1){
	 	//PutString("pas d'Erreur lors de la création du thread\n");
	 	PutString("Je suis le second !\n");
	 }
	
	 
	Halt();	
	//PutChar('z');
	return 0;
}
