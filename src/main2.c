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

#define THREAD_APTO 0
#define THREAD_EXEC 1
#define THREAD_BLOQ 2
#define THREAD_TERM 3
#define stackLength SIGSTKSZ

FILA2 fila;
char pilha[stackLength];
ucontext_t mainContext, contextDispatcher, contextClear;
TCB_t* currentTCB;

void PA() {
    int i, r = 4, n = 0;
    for (i = 1; i <= NUM_PA; ++i) {
        printf("PA ");
        n+=r;
        printf("i: %d n: %d", i, n);
        swapcontext(&currentTCB->context, &contextDispatcher);
    }
}
void PG() {
    int i, r = 2, n = 1;
    for (i = 1; i <= NUM_PG; ++i) {
        printf("PG ");
        n*=r;
        printf("i: %d n: %d", i, n);
        swapcontext(&currentTCB->context, &contextDispatcher);
    }
}
void fibonacci() {
    int f, f0 = 1, f1 = 1, i;
    for (i = 2; i <= NUM_FIBONACCI; ++i)    {       
        printf("fib  ");
        f = f0 + f1;
        f0 = f1;
        f1 = f; 
        printf("i: %d f: %d", i, f);
        swapcontext(&currentTCB->context, &contextDispatcher);
    }
}
void triangulo() {
    int t = 0, i;
    for (i = 1; i <= NUM_TRIANGULO; i++) {      
        printf("tri ");
        t += (i-1);
        printf("i: %d t: %d", i, t);
        swapcontext(&currentTCB->context, &contextDispatcher);
    }
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

void clear() {
    if(!FirstFila2(&fila)) {
        do {
            TCB_t* deleted = GetAtIteratorFila2(&fila);
            if(deleted->tid == currentTCB->tid) {
                DeleteAtIteratorFila2(&fila);
                free(deleted->context.uc_stack.ss_sp);
                free(deleted);
                currentTCB = NULL;
                break;
            }
        } while(NextFila2(&fila)==0);
        
    }
}
void terminate() {
    clear();
    setcontext(&contextDispatcher);
}
void dispatch(){
    if(!FirstFila2(&fila)) {
        currentTCB = GetAtIteratorFila2(&fila);
        DeleteAtIteratorFila2(&fila);
        AppendFila2(&fila, (void *) currentTCB);
        printf("\ntcb: %d ", currentTCB->tid);
        setcontext(&currentTCB->context);
    }
    
}

int main (int argc, char** argv) {
    void* funcs[4] = {PA, PG, fibonacci, triangulo}; 
    
    
    int criou = CreateFila2(&fila), ret_code = 1;
    if(criou!=0){
        printf("Erro ao criar a fila \n");      
        exit(1);
    }
    
    

    int i;
    for(i = 0 ; i < 4; i++) {        
        ucontext_t context;
        getcontext(&context);
        context.uc_stack.ss_sp = (char *) malloc(stackLength);
        context.uc_stack.ss_size = stackLength;
        context.uc_link =  &contextClear;
        makecontext(&context, (void (*)(void))funcs[i],0);

        TCB_t* novoTCB = malloc(sizeof(TCB_t));
        novoTCB->tid = i;
        novoTCB->context = context;
        AppendFila2(&fila, (void *) novoTCB);
    }

    
    //dispatcher
    getcontext(&contextDispatcher);
    contextDispatcher.uc_stack.ss_sp = (char *) malloc(stackLength);     
    contextDispatcher.uc_stack.ss_size = stackLength;
    contextDispatcher.uc_link = &mainContext;
    makecontext(&contextDispatcher, (void (*)(void))dispatch, 0);    

    getcontext(&contextClear);
    contextClear.uc_stack.ss_sp = (char *) malloc(stackLength);     
    contextClear.uc_stack.ss_size = stackLength;
    contextClear.uc_link = 0;
    makecontext(&contextClear, (void (*)(void))terminate, 0);    

    getcontext(&mainContext);
    if(ret_code){        
        ret_code = 0;
        setcontext(&contextDispatcher);
    }    
    printf("\nsaiu da main\n");
    return 0;
}