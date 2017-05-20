#include "../include/support.h"
#include "../include/cthread.h"
#include <stdio.h>

void* func0(void *arg) {
	printf("Deve aparecer antes\n");
	return;
}

void* func1(void *arg) {
	printf("Deve aparecer depois\n");
}

int main(int argc, char *argv[]) {

	int	id0, id1;
	int j0, j1;
	int i;

	id0 = ccreate(func0, (void *)&i,2); //tem menor prioridade
	id1 = ccreate(func1, (void *)&i,1); //tem maior prioridade

	csetprio(id0,0); //agora o id0 tem maior prioridade
	j0 = cjoin(id0);
	j1 = cjoin(id1);
	printf("Saindo da main\n");
	return 1;
}