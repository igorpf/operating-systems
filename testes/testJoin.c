#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>

void* func0(void *arg) {
	printf("Eu sou a thread ID0 imprimindo %d\n", *((int *)arg));
	return;
}

void* func1(void *arg) {
	printf("Eu sou a thread ID1 imprimindo %d\n", *((int *)arg));
}

int main(int argc, char *argv[]) {

	int	id0, id1;
	int j0, j1;
	int i;

	id0 = ccreate(func0, (void *)&i,1);
	id1 = ccreate(func1, (void *)&i,0);

	j0 = cjoin(id0);
	j1 = cjoin(id1);


}