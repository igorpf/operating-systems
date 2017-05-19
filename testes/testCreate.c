/**
** Teste da função ccreate
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"
csem_t          mutex;
void* func0(void *arg) {
	printf("Antes de esperar mutex\n");
	cwait(&mutex);
	printf("Depois de esperar mutex\n");
	return;
}


int main(int argc, char *argv[]) {

	int	id0;//, id1, id2, id3, id4, id5, id6;
	int i;
	csem_init(&mutex, 1);
	
	id0 = ccreate(func0, (void *)&i,0);
	cjoin(id0);
	return 0;
}
