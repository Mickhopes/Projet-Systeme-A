#include "syscall.h"
#include "errorno.h"
#define NULL 0


void attendre(void* arg) {
	
	PutString("J'attends le thread #");
	PutInt((int) arg);
	PutChar('\n');

	if (UserThreadJoin((int) arg) == -1) {
		int error = GetErrorNo();
		if (error == ESRCH) {
			PutString("Thread introuvable\n");
		} else if (error == EDEADLCK) {
			PutString("Deadlock detecte\n");
		}
	}
	PutChar('\n');
	
	UserThreadExit();
}	

void print(void *arg){
	int i = 0;
	while(i<100)
	{
		PutChar('.');
		i++;
	}
	PutChar('\n');

	UserThreadExit();
}


int main(){
	int id;

	if((id = UserThreadCreate(print, NULL)) != -1){
		PutString("Je suis le premier !\n");
	}
	if(UserThreadCreate(attendre,(void*) 3) != -1){
		PutString("Je suis le second !\n");
	}
	if(UserThreadCreate(attendre,(void*) 2) != -1){
		PutString("Je suis le troisieme !\n");
	}
	
	return 0;
}
