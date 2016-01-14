#include "syscall.h"
#include "errorno.h"
#define NULL 0


void print(void* arg) {
	
	int i;
	for (i = 0; i < 10; i++){
		PutChar((int)arg + i);	
	}
}


int main(){

	if((UserThreadCreate(print, (void*)'A')) != -1){

	}
	if(UserThreadCreate(print, (void*)'a') != -1){
		
	}
	
	return 0;
}
