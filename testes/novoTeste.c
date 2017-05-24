/*
	A ideia desse teste é forçar o cjoin
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/support.h"
#include "../include/cthread.h"

#define MAXTHR 2100
csem_t	mutex;

void uselessFunc(int tid){
	
	cwait(&mutex);
	switch(tid){
		case 1: 
			csignal(&mutex);
			cjoin(MAXTHR);
			break;
		case 100:
			csignal(&mutex);
			break;
		default:
			csignal(&mutex);
			cjoin(tid - 1);

	}
	

	printf("Thread %i\n", tid);
}

int main(int argc, char **argv)
{
  	int i = 0;
  	csem_init(&mutex, 1);

  	cwait(&mutex);
	for(i = 0; i < MAXTHR; i++){
		ccreate((void *)uselessFunc, (void *)(i+1), 1);
	}
	csignal(&mutex);
	
	cjoin (MAXTHR - 1);

	printf("Main retornando para finalizar o programa.\n\n");

  	return 0;
}