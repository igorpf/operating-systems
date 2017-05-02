#include <stdio.h>
#include <stdlib.h>
#include "../include/support.h"
#include "../include/cdata.h"

#define APTO 0
#define EXECUTANDO 1
#define BLOQUEADO 2
#define NUM_FILAS 3
#define NUM_TCB_CRIADOS 30

FILA2 filas[NUM_FILAS];
int terminados = 0;

int main (int argc, char** argv) {
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
void printMensagem(int iteracao, int resto) {
    const char* explicacao;
    switch(resto) {
        case 0:
        case 6:
            explicacao = "Criação de elemento TCB";
            break;
        case 1:
        case 7:
            explicacao = "Colocando apto em execução";
            break;
        case 2:
        case 8:
            explicacao = "Término de execução de thread";
            break;
        case 3:
        case 9:
            explicacao = "Mover de executando para bloqueado";
            break;
        case 4:
        case 10:
            explicacao = "Mover de executando para apto (yield)";
            break;    
        case 5:
        case 11:
            explicacao = "Mover de bloqueado para apto";
            break;
    }
    printf("Iteração %d\tResto: %d\t Ação: %s\n", iteracao, resto, explicacao);
    printf("\tElementos em apto: %d", 0);
    printf("\tElementos em executando: %d", 0);
    printf("\tElementos em bloqueado: %d",0);
    printf("\tElementos terminados: %d",0);
}