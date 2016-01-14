#include "syscall.h"

#define M 4
#define NbThreads 6
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
    //UserThreadExit();
}

void recharger_glace() {
    int recharge = 1;
    while(recharge) {
        P(dring);
        PutString("le glacier est reveille\n");
        if (nb_glaces == 0) {
            nb_glaces += M;
            recharge --;
            PutString("le glacier a rechargé\n");
        }
        PutString("le glacier libere et se rendort\n");
        V(vente);
    }
    //UserThreadExit();
}

int main (){

    dring = InitSemaphore("dring", 0);
    vente = InitSemaphore("vente", 1);
    nb_glaces = M;

    int i;
    for (i = 1; i < NbThreads; i++){
        UserThreadCreate(acheter_glace, 0);
    }
    UserThreadCreate(recharger_glace, 0);

    /* Wait until every thread ened */ 
    for (i = 1; i <= NbThreads; i++){
        UserThreadJoin(i);
    }

    return 0;
}