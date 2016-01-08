#include "syscall.h"
#define NULL 0

void print(void *i){
	//PutChar('Q');
	 int var = 0;
	 var=1;
	 
	 int i;
	 for (i = 0; i < 100; i++){
		var ++;
	 }
	 PutChar((char) i);
	UserThreadExit();
}	


int main(){
	
	if(UserThreadCreate(print, (void *) 'a') != -1){
		//PutString("pas d'Erreur lors de la création du thread\n");
		PutChar('z');
	}
	if(UserThreadCreate(print, (void *) 'b') != -1){
	 	//PutString("pas d'Erreur lors de la création du thread\n");
	 	PutChar('p');
	 }
	 if(UserThreadCreate(print, (void *) 'c') != -1){
	 	//PutString("pas d'Erreur lors de la création du thread\n");
	 	PutChar('m');
	 }

		
	//PutChar('z');
	return 0;
}
