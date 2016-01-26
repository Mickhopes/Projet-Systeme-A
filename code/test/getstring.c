#include "syscall.h"

int
main()
{
    char chaine[10];
    GetString(chaine,9);
    PutString("Result : ");
    PutString(chaine);
    PutString("\n");
    return 0;
}
