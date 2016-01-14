#include "syscall.h"

#define M 4
#define NbThreads 10

int nb_glaces;
sem_t dring;
sem_t vente;

void acheter_glace() {
    P(vente);
    nb_glaces--;
    PutString("Achat d'1 glace : ");
    PutInt(nb_glaces);
    PutChar('\n');
    V(dring);
}

void recharger_glace() {
    while(1) {
        P(dring);
        PutString("le glacier est reveille\n");
        if (nb_glaces == 0) {
            nb_glaces += M;
            PutString("le glacier a rechargé\n");
        }
        PutString("le glacier libere et se rendort\n");
        V(vente);
    }
}

int main (){

    dring = InitSemaphore("dring", 0);
    vente = InitSemaphore("vente", 1);
    nb_glaces = M;

    UserThreadCreate(recharger_glace, 0);
    int i;
    for (i = 0; i < NbThreads; i++){
        UserThreadCreate(acheter_glace, 0);
    }

    /* Wait until every thread ened */ 
    for (i = 1; i <= NbThreads+1; i++){
        UserThreadJoin(i);
    }

    return 0;
}