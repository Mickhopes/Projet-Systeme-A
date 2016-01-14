#include "syscall.h"

#define M 4
#define NB_THREAD 5

int nb_glaces;
sem_t dring;
sem_t vente;

void acheter_glace() {
    P(vente);
    nb_glaces--;
    PutString("Achat d'1 glace : ");
    PutInt(nb_glaces);
    PutChar('\n');
    //printf("thread %u a acheté une glace. (nombre restant = %d)\n", (unsigned int) pthread_self(), nb_glaces);
    //sleep(1);
    V(dring);
}

void recharger_glace() {
    while(1) {
        P(dring);
        PutString("le glacier est reveille\n");
        if (nb_glaces == 0) {
            nb_glaces += M;
            //printf("le glacier a rechargé\n");
            PutString("le glacier a rechargé\n");
            //sleep(1);
        }
        V(vente);
    }
}

int main (){

    dring = InitSemaphore("dring", 0);
    vente = InitSemaphore("vente", 1);
    nb_glaces = M;

    int i;
    for (i = 1; i < NB_THREAD; i++){
        UserThreadCreate(acheter_glace, 0);
    }
    UserThreadCreate(recharger_glace, 0);

    /* Wait until every thread ened */ 
    for (i = 1; i < NB_THREAD; i++){
        UserThreadJoin(i);
    }

    return 0;
}