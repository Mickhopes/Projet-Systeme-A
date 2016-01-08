#include "syscall.h"
#define NULL 0

void print(){
	//PutChar('Q');
	 int var = 0;
	 var=1;
	 
	 int i;
	 for (i = 0; i < 100; i++){
		var ++;
	 }
	UserThreadExit();
}	


int main(){
	
	if(UserThreadCreate(print, NULL) != -1){
		//PutString("pas d'Erreur lors de la création du thread\n");
		PutChar('z');
	}
	if(UserThreadCreate(print, NULL) != -1){
	 	//PutString("pas d'Erreur lors de la création du thread\n");
	 	PutChar('p');
	 }
	 if(UserThreadCreate(print, NULL) != -1){
	 	//PutString("pas d'Erreur lors de la création du thread\n");
	 	PutChar('m');
	 }

		
	//PutChar('z');
	return 0;
}
