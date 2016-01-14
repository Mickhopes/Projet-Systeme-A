#include "syscall.h"

int varGlob = 0;
sem_t mutex;

void incremente(void* arg) {
	int val = (int)arg;
	//sem
	P(mutex);
	int i = 0;
	for (i = 0; i < val; i++){
		varGlob++;
	}
	V(mutex);
	//fin sem
	
	UserThreadExit();
}	


int main(){

	int id[3];
	int i;
	mutex = InitSemaphore("mutex",1);
	id[0] = UserThreadCreate(incremente,(void*)  125);
	id[1] = UserThreadCreate(incremente,(void*)  172);
	id[2] = UserThreadCreate(incremente,(void*)  147);

	for (i = 0; i < 3; i++){
		UserThreadJoin(id[i]);	
	}
	DestroySemaphore(mutex);

	PutString("Resultat attendu : 444, reel : ");
	PutInt(varGlob);
	PutChar('\n');
	
	return 0;
}
