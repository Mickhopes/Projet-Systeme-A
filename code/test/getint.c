#include "syscall.h"

int
main()
{
    int n = GetInt();
    PutInt(n+42);
    PutChar('\n');
    
    return 0;
}
