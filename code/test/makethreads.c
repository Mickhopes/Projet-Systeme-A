#include "syscall.h"

void print(void* arg) {
	PutChar((char)((int)arg));
	PutString("coucou !");

	UserThreadExit();
}	


int main(){
	
	if(UserThreadCreate(print, (void*) 'a') != -1){
		//PutString("pas d'Erreur lors de la création du thread\n");
		PutString("Je suis le premier !\n");
	}
	if(UserThreadCreate(print, (void*) 'b') != -1){
	 	//PutString("pas d'Erreur lors de la création du thread\n");
	 	PutString("Je suis le second!\n");
	 }
	 if(UserThreadCreate(print, (void*) 'c') != -1){
	 	//PutString("pas d'Erreur lors de la création du thread\n");
	 	PutString("Je sais pas, je sais pas compter !\n");
	 }

		
	//PutChar('z');
	return 0;
}
