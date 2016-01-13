#include "syscall.h"
#include "errorno.h"
#define NULL 0

int varGlob = 0;
sem_t *mutex;

void incremente(void* arg) {
	
	//sem
	P(mutex);
	for (int i = 0; i < 5; i++){
		varGlob++;
	}
	V(mutex);
	//fin sem
	
	UserThreadExit();
}	


int main(){

	int id[3];
	mutex = InitSemaphore("mutex",1);
	if((id[0] = UserThreadCreate(incremente,(void*)  5)) != -1){
		//PutString("Je suis le premier !\n");
	}
	if((id[1] = UserThreadCreate(incremente,(void*)  3)) != -1){
		//PutString("Je suis le second !\n");
	}
	if((id[2] = UserThreadCreate(incremente,(void*)  4)) != -1){
		//PutString("Je suis le troisieme !\n");
	}

	for (int i = 0; i < 3; i++){
		UserThreadJoin(id[i]);	
	}
	DestroySemaphore(mutex);

	PutString("Resultat attendu : 12, reel : ");
	PutInt(varGlob);
	PutChar('\n');
	
	return 0;
}
