#include <stdio.h>
#include <stdlib.h>
#include "../include/support.h"
#include "../include/cthread.h"

int main(int argc, char *argv[]){
	char c[105] = "";
	int i = cidentify(c, sizeof(c));
	if(i == -1)
		printf("cidentify = %d: deu erro\n", i);
	else
		printf("cidentify = %d: deu certo\n", i);

	printf("Nomes:\n");
	while(i<sizeof(c)){
		printf("%c", c[i]);
		i++;
	}	


	return 0;
}
