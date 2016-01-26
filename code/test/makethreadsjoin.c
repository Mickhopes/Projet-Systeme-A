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
		} else if (error == EDEADLK) {
			PutString("Deadlock detecte\n");
		}
	}
	PutString("Le thread ");
	PutInt((int)arg);
	PutString(" s'est termine avant celui-ci.\n");
}	

void print(void *arg){
	int i = 0;
	while(i<100)
	{
		PutChar('.');
		i++;
	}
	PutChar('\n');
}


int main(){
	int id;

	if((id = UserThreadCreate(print, NULL)) == -1){
		PutString("Erreur lors de la création du thread: ");
		PutInt(GetErrorNo());
		PutChar('\n');
	}
	if(UserThreadCreate(attendre,(void*) id) == -1){
		PutString("Erreur lors de la création du thread: ");
		PutInt(GetErrorNo());
		PutChar('\n');
	}
	
	return 0;
}
