/*
Christian Schmitz - 242258
Igor Pires Ferreira - 242267
*/

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
int terminados = 0, criados = 0;


int sorteia() {
    return Random2()%12;
}
int tamanhoFila(FILA2 *fila) {
    int resultado = FirstFila2(fila);
    int tamanho = 0, continua = 1;
    if (resultado == 0)
        tamanho++;
    else
        continua = 0;
    while(continua) {
        resultado = NextFila2(fila);
        if(resultado==0)
            tamanho++;
        else 
            continua = 0;
    } 
    return tamanho;
}
TCB_t* criaTCB(){
    TCB_t* novo = malloc(sizeof(TCB_t));
    novo->tid = criados;
    return novo;
}
void inicializaFilas() {
    int i;
    for(i = 0; i < NUM_FILAS;i++) {
        int criou = CreateFila2(&filas[i]);
        if(criou!=0){
            printf("Erro ao criar a fila %d\n", i);
            exit(1);
        }

    } 
}
void printMensagem(int iteracao, int resto) {
    const char* explicacao;
    switch(resto) {
        case 0:
        case 6:
            explicacao = "Criacao de elemento TCB";
            break;
        case 1:
        case 7:
            explicacao = "Colocando apto em execucao";
            break;
        case 2:
        case 8:
            explicacao = "Termino de execucao de thread";
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
    // printf("Criados: %d\n", criados);
    printf("Iteracao: %d\tResto: %d\t Acao: %s", iteracao, resto, explicacao);
    printf("\n\tElementos em apto: %d", tamanhoFila(&filas[APTO]));
    printf("\n\tElementos em executando: %d", tamanhoFila(&filas[EXECUTANDO]));
    printf("\n\tElementos em bloqueado: %d",tamanhoFila(&filas[BLOQUEADO]));
    printf("\n\tElementos terminados: %d\n\n", terminados);
}

void realizaAcao(int resto) {
    int resultado;
    TCB_t* elemento;
    switch(resto) {
        case 0:
        case 6:
            elemento = criaTCB();
            resultado = AppendFila2(&filas[APTO], (void *) elemento);
            break;
        case 1:
        case 7:
            resultado = FirstFila2(&filas[EXECUTANDO]);
            // printf("Apto p exec: tamanhoFila: %d. resultado: %d\n", tamanhoFila(&filas[EXECUTANDO]), resultado);
            if(resultado!=0){ //fila vazia?
                resultado = FirstFila2(&filas[APTO]); 
                if (resultado == 0) { //existe alguém em apto?
                    //Remove de apto e coloca em executando
                    elemento = (TCB_t *) GetAtIteratorFila2(&filas[APTO]);
                    DeleteAtIteratorFila2(&filas[APTO]);
                    // printf("\nColocando emexecucao Id: %d\n", elemento->tid);
                    AppendFila2(&filas[EXECUTANDO], (void *) elemento);
                }
            }
            break;
        case 2:
        case 8:
            resultado = FirstFila2(&filas[EXECUTANDO]);
            if(resultado==0){ //Tem alguém executando?
                //Término de thread. Tira da fila e desaloca a memória.
                elemento = (TCB_t *) GetAtIteratorFila2(&filas[EXECUTANDO]);
                DeleteAtIteratorFila2(&filas[EXECUTANDO]);
                free(elemento);
                elemento = NULL;
                terminados++;
            }
            break;
        case 3:
        case 9:
            resultado = FirstFila2(&filas[EXECUTANDO]);
            if(resultado==0){ //Tem alguém executando?
                //Tira de executando e manda para bloqueado
                elemento = (TCB_t *) GetAtIteratorFila2(&filas[EXECUTANDO]);
                DeleteAtIteratorFila2(&filas[EXECUTANDO]);
                AppendFila2(&filas[BLOQUEADO], (void *) elemento);
            }
            // explicacao = "Mover de executando para bloqueado";
            break;
        case 4:
        case 10:
            resultado = FirstFila2(&filas[EXECUTANDO]);
            if(resultado==0){ //Tem alguém executando?
                //Tira de executando e manda para apto
                elemento = (TCB_t *) GetAtIteratorFila2(&filas[EXECUTANDO]);
                DeleteAtIteratorFila2(&filas[EXECUTANDO]);
                AppendFila2(&filas[APTO], (void *) elemento);
            }
            // explicacao = "Mover de executando para apto (yield)";
            break;    
        case 5:
        case 11:
            resultado = FirstFila2(&filas[BLOQUEADO]);
            if(resultado==0){ //Tem alguém executando?
                //Tira de bloqueado e manda para bloqueado
                elemento = (TCB_t *) GetAtIteratorFila2(&filas[BLOQUEADO]);
                DeleteAtIteratorFila2(&filas[BLOQUEADO]);
                AppendFila2(&filas[APTO], (void *) elemento);
            }
            // explicacao = "Mover de bloqueado para apto";
            break;
    }
}
//1 caso vazia, 0 caso contrário
int filasVazias() {
    int i;
    for(i=0; i < NUM_FILAS; i++) {
        int tamanho = tamanhoFila(&filas[i]);
        if (tamanho > 0)
            return 0;
    }
    return 1;
}

int main (int argc, char** argv) {
    
    inicializaFilas();
    int continua = 1, iteracao=0;
    while(continua) {
        iteracao++;
        int acao = sorteia();
        if ((acao == 0 || acao == 6) && criados >= NUM_TCB_CRIADOS)
            continue;
        if(acao == 0 || acao == 6)
            criados ++;
        realizaAcao(acao);
        printMensagem(iteracao, acao);
        if (criados >= NUM_TCB_CRIADOS && filasVazias()){
            continua = 0;            
        }
    }

    return 0;
}