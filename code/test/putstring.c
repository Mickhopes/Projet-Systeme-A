#include "syscall.h"

int
main()
{
    PutString("coucou\0kjgrkjdrgkvrejhgoieurhguioer");
    PutString("\nc");
    Halt();
}
