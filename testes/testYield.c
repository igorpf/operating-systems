#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/support.h"
#include "../include/cthread.h"

void* func0(void *arg) {
	printf("Eu sou uma THREAD imprimindo %d antes do yield\n", *(int *)arg);
	cyield();
	//não deveria chegar aqui
	return;
}

int main(int argc, char *argv[]) {

	int	id0;
	int i = 1;
	id0 = ccreate(func0, (void *)&i,0);

	cjoin(id0);
	printf("Final da main\n");
	return 1;
}
