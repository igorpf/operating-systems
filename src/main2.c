/*
Christian Schmitz - 242258
Igor Pires Ferreira - 242267
*/

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "../include/support.h"
#include "../include/cdata.h"

#define NUM_PA 8
#define NUM_PG 10
#define NUM_FIBONACCI 12
#define NUM_TRIANGULO 6

#define TEM_THREAD_NA_FILA 0
#define NAO_TEM_THREAD_NA_FILA 1

FILA2 fila;
ucontext_t mainContext;

void PA() {
    int i, r = 4, n = 0;
    for (i = 1; i <= NUM_PA; ++i) {
        n+=r;
    }
}
void PG() {
    int i, r = 2, n = 1;
    for (i = 1; i <= NUM_PG; ++i) {
        n*=r;
    }
}
void fibonacci() {
    int f, f0 = 1, f1 = 1, i;

    for (i = 2; i <= NUM_FIBONACCI; ++i)    {       
        f = f0 + f1;
        f0 = f1;
        f1 = f; 
        // printf("i: %d, f: %d\n", i, f);
    }
}
void triangulo() {
    int t = 0, i;

    for (i = 1; i <= NUM_TRIANGULO; i++) {      
        t += (i-1);
        // printf("i: %d, t: %d\n", i, t);
    }
}

TCB_t* criaTCBfuncPA(){ //cria TCB com contexto pra func PA
    TCB_t* novoTCB = malloc(sizeof(TCB_t));
    novoTCB->tid = 0;
    makecontext(&(novoTCB->context), (void*)&PA, 0);

    return novoTCB;
}

TCB_t* criaTCBfuncPG(){
    TCB_t* novoTCB = malloc(sizeof(TCB_t));
    novoTCB->tid = 0;
    makecontext(&(novoTCB->context), (void*)&PG, 0);

    return novoTCB;
}

TCB_t* criaTCBfuncFib(){ 
    TCB_t* novoTCB = malloc(sizeof(TCB_t));
    novoTCB->tid = 0;
    makecontext(&(novoTCB->context), (void*)&fibonacci, 0);

    return novoTCB;
}

TCB_t* criaTCBfuncTriang(){
    TCB_t* novoTCB = malloc(sizeof(TCB_t));
    novoTCB->tid = 0;
    makecontext(&(novoTCB->context), (void*)&triangulo, 0);

    return novoTCB;
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

void cedeuCPU(){

    TCB_t* TCBdaThreadNaCPU = (TCB_t *) GetAtIteratorFila2(&fila);
    getcontext(&(TCBdaThreadNaCPU->context));  
    DeleteAtIteratorFila2(&fila);
    AppendFila2(&fila, (void *) TCBdaThreadNaCPU);
    TCB_t* TCBdaNovaThreadNaCPU = (TCB_t *) GetAtIteratorFila2(&fila);
    setcontext(&(TCBdaNovaThreadNaCPU->context));

    //minha ideia era fazer essa func aqui pra rodar a cada iteracao das funcs matem'aticas, e ao final de cada uma,
    //elas chamarem um fimExecthread que retira a thread da fila e seta o contexto pra main se for a ultima 
    //thread ainda em exec
    
}

int main (int argc, char** argv) {
    // fibonacci();
    // triangulo();
    
    int criou = CreateFila2(&fila);
    if(criou!=0){
        printf("Erro ao criar a fila \n");      
        exit(1);
    }

    AppendFila2(&fila, (void *) criaTCBfuncPA());
    AppendFila2(&fila, (void *) criaTCBfuncPG());
    AppendFila2(&fila, (void *) criaTCBfuncFib());
    AppendFila2(&fila, (void *) criaTCBfuncTriang());

    getcontext(&mainContext);
    

    return 0;
}