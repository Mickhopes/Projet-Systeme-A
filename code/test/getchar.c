#include "syscall.h"

void print_user(int n)
{
    int i;
	char c;
    for (i = 0; i < n; i++) {
		c = GetChar();
		if (c != '\n') {
			PutChar('<');
        	PutChar(c);
			PutChar('>');
			PutChar('\n');
		} else {
			i--;
		}
    }
    PutChar('\n');
}

int
main()
{
    print_user(4);
    Halt();
}
