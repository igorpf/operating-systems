/*
Christian Schmitz - 242258
Igor Pires Ferreira - 242267
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
TCB_t* CPU, *mainThread;
int tid=1, initializedDeps = 0;

//Auxiliary functions
//---------------------------------------------------
int searchTCBQueue(FILA2* queue, int tid) {
    TCB_t *ptr;
    if(!FirstFila2(queue)) {
        ptr = GetAtIteratorFila2(queue);
        if(tid == ptr->tid) {
            return SUCCESS;
        } else {
            while(!NextFila2(queue)) {
                if(tid == ptr->tid) {
                    return SUCCESS;
                }
            }
        }
    }    
    return ERROR;
}
int searchJoinQueue(FILA2* queue, int tid) {
    JOIN *ptr;
    if(!FirstFila2(queue)) {
        ptr = GetAtIteratorFila2(queue);
        // printf("\nwaited: %d, waiting: %d\n", ptr->waitedTid, ptr->waitingTid);
        if(tid == ptr->waitedTid) {
            return SUCCESS;
        } else {
            while(!NextFila2(queue)) {
                if(tid == ptr->waitedTid) {
                    return SUCCESS;
                }
            }
        }
    }    
    return ERROR;
}
int verifyJoinedThreads(int waitedTid) {
    if(searchJoinQueue(&joinQueue, waitedTid) == SUCCESS && searchTCBQueue(&blockedQueue, waitedTid) == SUCCESS) {
        JOIN* ptr = (JOIN*) GetAtIteratorFila2(&joinQueue);        
        DeleteAtIteratorFila2(&joinQueue);
        free(ptr);
        ptr = NULL;
        
        TCB_t* freed = GetAtIteratorFila2(&blockedQueue);
        freed->state = THREAD_RDY;
        AppendFila2(&readyQueue[freed->ticket], (void *)freed);    
        DeleteAtIteratorFila2(&blockedQueue);
    }
    return 0;
}
int generateId() {
    return tid++;
}

void terminate() {
    //TODO: verify join
    // printf("Terminate\n");
    verifyJoinedThreads(CPU->tid);
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
    setcontext(&mainThread->context);
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
    int success = !CreateFila2(&blockedQueue) && !CreateFila2(&joinQueue), i;
    for(i = 0 ; i < READY_QUEUES; i++)
        success = success && !CreateFila2(&readyQueue[0]);    
    return success;

}
int initMainThread() {
    mainThread = (TCB_t *) malloc(sizeof(TCB_t));
    mainThread->tid = 0;
    mainThread->state = THREAD_EXEC;
    mainThread->ticket = 0;
    getcontext(&mainThread->context);
    CPU = mainThread;
    return 0;
}
void init() {
    initQueues();
    initMainThread();
    initTerminator();
    initDispatcher();    
}
/***********************************************************
********************* Cthread   ****************************

**********************************************************/
//------------------------------------------------------
int ccreate (void* (*start)(void*), void *arg, int prio) {
    if(!initializedDeps) {        
        init();
        initializedDeps = 1;
    }

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
        newTCB->ticket = prio;
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
    if(!initializedDeps) {        
        init();
        initializedDeps = 1;
    }

    if (CPU) {
        CPU->state = THREAD_RDY;
        AppendFila2(&readyQueue[CPU->ticket], (void *) CPU);
        swapcontext(&CPU->context, &contextDispatcher); 
        return 0;
    }
    return ERROR;
}

int cjoin(int tid) {
    // First, check if the thread exists
    int i, found = 0;
    if(searchTCBQueue(&blockedQueue, tid) == ERROR) {        
        for(i = 0; i < READY_QUEUES; i++) {            
            if(searchTCBQueue(&readyQueue[i], tid) == SUCCESS) {
                found = 1;
                break;
            }
        }
        
        if(!found)
            return ERROR;
    }
    
    //Then, check if the thread isn't being waited for
    if (searchJoinQueue(&joinQueue,tid) == SUCCESS)
        return ERROR;
    //Block the current thread and send it to the joinQueue
    if (CPU) {
        CPU->state = THREAD_BLOC;
        JOIN *newJoinPair = (JOIN *) malloc(sizeof(JOIN));
        newJoinPair->waitedTid = tid;
        newJoinPair->waitingTid = CPU->tid;
        if(!AppendFila2(&blockedQueue, (void *) CPU) && !AppendFila2(&joinQueue, (void *) newJoinPair)) {            
            swapcontext(&CPU->context, &contextDispatcher); 
            return 0;    
        }        
    }
    return ERROR;
}
int csem_init(csem_t *sem, int count) {
    if(!initializedDeps) {        
        init();
        initializedDeps = 1;
    }
    sem->count = count;
    sem->fila = 0;
    // printf("Criando semaforo\n");
    if(sem)
        return 0;
    else
        return ERROR;
}

int cwait(csem_t *sem) {
    if(!initializedDeps) {        
        init();
        initializedDeps = 1;
    }

    if(!sem)
        return ERROR;
    sem->count--;
    
    if(sem->count < 0) {
        if(!sem->fila) {
            sem->fila = (FILA2 *) malloc(sizeof(FILA2));
            if(CreateFila2(sem->fila))
                return ERROR;
        } 
        CPU->state = THREAD_BLOC;    
        if(AppendFila2(&blockedQueue, (void *) CPU) || AppendFila2(sem->fila, (void *) CPU))
            return ERROR;

        swapcontext(&CPU->context, &contextDispatcher);
    }

    return 0;
}

int csignal(csem_t *sem) {
    if(!initializedDeps) {        
        init();
        initializedDeps = 1;
    }

    if(!sem)
        return ERROR;
    sem->count++;
    
        // printf("Liberando alguém\n");
    if(sem->count <= 0) {
        if(!sem->fila) {
            sem->fila = (FILA2 *) malloc(sizeof(FILA2));
            if(CreateFila2(sem->fila))
                return ERROR;
        } 
        if(!FirstFila2(sem->fila))
            return ERROR;
        TCB_t* freed = GetAtIteratorFila2(sem->fila);
        freed->state = THREAD_RDY;
        if(searchTCBQueue(&blockedQueue, freed->tid))
            return ERROR;
        DeleteAtIteratorFila2(sem->fila);
        DeleteAtIteratorFila2(&blockedQueue);            
    }
    return 0;
}

int cidentify (char *name, int size) {
    char* names = "Christian Schmitz 242258\nIgor Pires Ferreira 242267\n";
    if(strlen(names) > size) 
        return ERROR;
    strncat(name, names, size);
    return 0;
}

// int main(){char name[100];cidentify(name, 100); return 0;}