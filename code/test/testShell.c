#include "syscall.h"

int main (){
    char buffer[60];
    int i;

    while (1){
    	PutString("user@nachos $ ");

		i = 0;

		do{
			buffer[i] = GetChar();
		}while (buffer[i++] != '\n');

        buffer[--i] = '\0';

        if(buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'i' && buffer[3] == 't' && buffer[4] == '\0'){
            return 0;
        }

    	ForkExec(buffer);
        Waitpid();
        PutChar('\n');
    }
    return 0;
}
