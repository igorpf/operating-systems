/*
	A ideia desse teste é forçar o cjoin, cyield e prioridades
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/support.h"
#include "../include/cthread.h"

#define MAXTHR 100
csem_t	mutex;

void uselessFunc(int tid){
	
	cwait(&mutex);
	

	int i = 0;
	for(i = 0; i < 4; i ++){
		csignal(&mutex);
		csetprio(tid, i);
		printf("\nExecutando thread %i, na iteracao %i", tid, i);
		cyield();
	}
	

	printf("Thread %i\n", tid);
}

int main(int argc, char **argv)
{
  	int i = 0;
  	csem_init(&mutex, 1);

  	cwait(&mutex);
	for(i = 0; i < MAXTHR; i++){
		ccreate((void *)uselessFunc, (void *)(i+1), 0);
	}
	csignal(&mutex);
	
	cjoin (MAXTHR - 1);

	printf("\nMain retornando para finalizar o programa.\n\n");

  	return 0;
}