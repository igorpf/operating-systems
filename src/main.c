#include <stdio.h>
#include <stdlib.h>
#include "../include/support.h"
#include "../include/cdata.h"

#define APTO 0
#define EXECUTANDO 1
#define BLOQUEADO 2
#define NUM_FILAS 3
#define NUM_TCB_CRIADOS 30

int main (int argc, char** argv) {

    FILA2 filas[NUM_FILAS];

    int i;
    for(i = 0; i < NUM_FILAS;i++) {
        int criou = CreateFila2(&filas[i]);
        if(criou!=0){
            printf("Erro ao criar a fila %d\n", i);
            return 1;
        }

    } 
    
    

    return 0;
}

int sorteia() {
    return Random2()%12;
}
TCB_t* criaTCB(){
    return malloc(sizeof(TCB_t));
}
