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

FILA2 executionQueue;
ucontext_t mainContext, contextDispatcher, contextClear;
TCB_t* currentTCB;
int a = 0, b = 0, f = 0, t = 0, ia, ib, ifi, it;

void yield() {
    AppendFila2(&executionQueue, (void *) currentTCB);
    swapcontext(&currentTCB->context, &contextDispatcher);
}

void printResults() {
    printf("----------------------------\n" );
    printf("PA termo   %d  : %d\n", ia, a);
    printf("PG termo   %d  : %d\n", ib, b);
    printf("Fibo termo %d  : %d\n", ifi, f);
    printf("Tri termo  %d  : %d\n", it, t);
}

void PA() {
    int r = 4;
    for (ia = 1; ia <= NUM_PA; ++ia) {
        // printf("PA ");
        a+=r;
        // printf("i: %d n: %d", i, n);
        yield();
    }
}
void PG() {
    int  r = 2;
    for (ib = 1; ib <= NUM_PG; ++ib) {
        // printf("PG ");
        b*=r;
        // printf("i: %d n: %d", i, n);
        yield();
        // swapcontext(&currentTCB->context, &contextDispatcher);
    }
}
void fibonacci() {
    int f0 = 1, f1 = 1;
    for (ifi = 2; ifi <= NUM_FIBONACCI; ++ifi)    {       
        // printf("fib  ");
        f = f0 + f1;
        f0 = f1;
        f1 = f; 
        // printf("i: %d f: %d", i, f);
        yield();
        //swapcontext(&currentTCB->context, &contextDispatcher);
    }
}
void triangulo() {
    for (it = 1; it <= NUM_TRIANGULO; it++) {      
        // printf("tri ");
        t += (it-1);
        // printf("i: %d t: %d", i, t);
        yield();
        // swapcontext(&currentTCB->context, &contextDispatcher);
    }
}


void clear() {
    free(currentTCB->context.uc_stack.ss_sp);
    free(currentTCB);
    currentTCB = NULL;
}
void terminate() {
    clear();
    setcontext(&contextDispatcher);
}
void dispatch(){
    if(!FirstFila2(&executionQueue)) {
        currentTCB = GetAtIteratorFila2(&executionQueue);
        DeleteAtIteratorFila2(&executionQueue);        
        // printf("\ntcb: %d ", currentTCB->tid);
        printResults();
        setcontext(&currentTCB->context);
    }
    
}


int main (int argc, char** argv) {
    void* funcs[4] = {PA, PG, fibonacci, triangulo}; 
    int criou = CreateFila2(&executionQueue), ret_code = 1;
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
        AppendFila2(&executionQueue, (void *) novoTCB);
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
    printResults();
    printf("\nsaiu da main\n");
    return 0;
}