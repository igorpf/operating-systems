/*
Christian Schmitz - 242258
Igor Pires Ferreira - 242267
*/

#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include "../include/support.h"
#include "../include/cdata.h"
#include "../include/cthread.h"

// 0: Criação; 1: Apto; 2: Execução; 3: Bloqueado e 4: Término
#define THREAD_CRE 0
#define THREAD_RDY 1
#define THREAD_EXEC 2
#define THREAD_BLOC 3
#define THREAD_TERM 4
#define stackLength SIGSTKSZ

#define SUCCESS 1
#define ERROR -1

#define READY_QUEUES 4
FILA2 readyQueue[READY_QUEUES], blockedQueue //TCB_t queues
        ,joinQueue; // JOIN queues
ucontext_t mainContext, contextDispatcher, contextClear;
TCB_t* CPU;
int tid=1;

//Auxiliary functions
//---------------------------------------------------
int generateId() {
    return tid++;
}

void terminate() {
    //TODO: verify join
    free(CPU->context.uc_stack.ss_sp);
    free(CPU);
    CPU = NULL;
    setcontext(&contextDispatcher);
}
void dispatch(){
    int i;
    for(i = 0 ; i < READY_QUEUES; i++) {
        if(!FirstFila2(&readyQueue[i])) {
            CPU = GetAtIteratorFila2(&readyQueue[i]);
            DeleteAtIteratorFila2(&readyQueue[i]);        
            setcontext(&CPU->context);
        }
    }
}
void initDispatcher() {
    getcontext(&contextDispatcher);
    contextDispatcher.uc_stack.ss_sp = (char *) malloc(stackLength);     
    contextDispatcher.uc_stack.ss_size = stackLength;
    contextDispatcher.uc_link = &mainContext;
    makecontext(&contextDispatcher, (void (*)(void))dispatch, 0);    
}

void initTerminator() {
    getcontext(&contextClear);
    contextClear.uc_stack.ss_sp = (char *) malloc(stackLength);     
    contextClear.uc_stack.ss_size = stackLength;
    contextClear.uc_link = 0;
    makecontext(&contextClear, (void (*)(void))terminate, 0);
}

int initQueues() {
    int success = !CreateFila2(&blockedQueue), i;
    for(i = 0 ; i < READY_QUEUES; i++)
        success = success && !CreateFila2(&readyQueue[0]);
    return success;

}
void init() {
    initQueues();
    initTerminator();
    initDispatcher();
}

//Cthread
//------------------------------------------------------
int ccreate (void* (*start)(void*), void *arg, int prio) {
    if(prio>=0 || prio<=3) {
        ucontext_t context;
        getcontext(&context);
        context.uc_stack.ss_sp = (char *) malloc(stackLength);
        context.uc_stack.ss_size = stackLength;
        context.uc_link =  &contextClear;
        makecontext(&context, (void (*)(void)) start, 1, arg);

        TCB_t* newTCB = malloc(sizeof(TCB_t));
        newTCB->tid = generateId();
        newTCB->state = THREAD_RDY;
        newTCB->context = context;
        newTCB->prio = prio;
        AppendFila2(&readyQueue[prio], (void *) newTCB);
        if(context.uc_stack.ss_sp && newTCB) //check if memory was correctly allocated
            return newTCB->tid;
        
        free(context.uc_stack.ss_sp);
        free(newTCB);       
        return ERROR;
    }//invalid priority
    return ERROR;
}
int csetprio(int tid, int prio) {
    if(prio < 0 || prio > 3)
        return ERROR;
    int i, found = 0;
    TCB_t *tcb;
    for(i = 0; i < READY_QUEUES; i++) {
        if(found)
            break;
        if(!FirstFila2(&readyQueue[i])) {
            tcb = (TCB_t *) GetAtIteratorFila2(&readyQueue[i]);
            if(tid == tcb->tid) {
                found = 1;
                DeleteAtIteratorFila2(&readyQueue[i]);
                AppendFila2(&readyQueue[prio], (void *) tcb);
            } else {
                while(!NextFila2(&readyQueue[i])) {
                    if(tid == tcb->tid) {
                        found = 1;
                        DeleteAtIteratorFila2(&readyQueue[i]);
                        AppendFila2(&readyQueue[prio], (void *) tcb);
                    }
                }
            }
        }
    }
    return found? 0:ERROR;
}

int cyield(void) {
    if (CPU) {
        CPU->state = THREAD_RDY;
        AppendFila2(&readyQueue[CPU->prio], (void *) CPU);
        swapcontext(&CPU->context, &contextDispatcher); 
        return 0;
    }
    return ERROR;
}

int cjoin(int tid) {
    return 0;
}
int csem_init(csem_t *sem, int count);
int cwait(csem_t *sem);
int csignal(csem_t *sem);
int cidentify (char *name, int size);

int main(){return 0;}