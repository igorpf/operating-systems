/**
** Teste da função ccreate
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"

void* func0(void *arg) {
	printf("Estou estou na func0 printando %d\n", (int)*((int*)arg));
	return arg;
}


int main(int argc, char *argv[]) {

	int	id0;
	int i=9;
	
	id0 = ccreate(func0, (void *)&i,0);
	cjoin(id0);
	return 0;
}
