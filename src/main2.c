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
char pilhas[4][SIGSTKSZ];
ucontext_t mainContext, mainContext2;

void PA(ucontext_t* this, ucontext_t *next) {
    printf("PA ");
    int i, r = 4, n = 0;
    for (i = 1; i <= NUM_PA; ++i) {
        n+=r;
        printf("i: %d n: %d\n", i, n);
        swapcontext(this, next);
        printf("HAY\n");
        // setcontext(next);
    }
}
void PG(ucontext_t* this, ucontext_t *next) {
    printf("PG ");
    int i, r = 2, n = 1;
    for (i = 1; i <= NUM_PG; ++i) {
        n*=r;
        printf("i: %d n: %d\n", i, n);
        swapcontext(this, next);
    }
}
void fibonacci(ucontext_t* this, ucontext_t *next) {
    int f, f0 = 1, f1 = 1, i;
printf("fib  ");
    for (i = 2; i <= NUM_FIBONACCI; ++i)    {       
        f = f0 + f1;
        f0 = f1;
        f1 = f; 
        printf("i: %d f: %d\n", i, f);
        swapcontext(this, next);
        // printf("i: %d, f: %d\n", i, f);
    }
}
void triangulo(ucontext_t* this, ucontext_t *next) {
    int t = 0, i;
printf("tri ");
    for (i = 1; i <= NUM_TRIANGULO; i++) {      
        t += (i-1);
        printf("i: %d t: %d\n", i, t);
        swapcontext(this, next);
        // printf("i: %d, t: %d\n", i, t);
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
    void* funcs[4] = {PA, PG, fibonacci, triangulo}; 
    
    
    int criou = CreateFila2(&fila);
    if(criou!=0){
        printf("Erro ao criar a fila \n");      
        exit(1);
    }
    
    

    int i;
    for(i = 0 ; i < 4; i++) {        
        ucontext_t context;
        getcontext(&context);
        context.uc_stack.ss_sp = pilhas[i];
        context.uc_stack.ss_size = sizeof(pilhas[i]);
        context.uc_link =  &mainContext;
        makecontext(&context, (void (*)(void))funcs[i], 2, &context, &mainContext);

        TCB_t* novoTCB = malloc(sizeof(TCB_t));
        novoTCB->tid = i;
        novoTCB->context = context;
        AppendFila2(&fila, (void *) novoTCB);
    }

    
    //dispatcher    
    while(tamanhoFila(&fila)) {
        // printf("s ");
        TCB_t* tcb; 
        getcontext(&mainContext);
        if(!FirstFila2(&fila)) {
            tcb = GetAtIteratorFila2(&fila);
            DeleteAtIteratorFila2(&fila);
            AppendFila2(&fila, (void *) tcb);
            printf("tcb: %d ", tcb->tid);
            swapcontext(&mainContext2, &tcb->context);            
            // printf("s2 ");
        }

    }

    return 0;
}