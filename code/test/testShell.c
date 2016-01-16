#include "syscall.h"

int main (){
    char buffer[60];
    int i/*,id*/;

    while (1){
    	PutString("user@nachos >");

		i = 0;

		do{
			buffer[i] = GetChar();
		}while (buffer[i++] != '\n');

		buffer[--i] = '\0';
    	/*id = */ForkExec(buffer);
		//waitpid (id);
    }
    return 0;
}
