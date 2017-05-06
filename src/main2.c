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

void PA() {
    int i, r = 4, n = 0;
    for (i = 1; i <= NUM_PA; ++i) {
        n+=r;
    }
}
void PG() {
    int i, r = 2, n = 1;
    for (i = 1; i <= NUM_PG; ++i) {
        n*=r;
    }
}
void fibonacci() {
    int f, f0 = 1, f1 = 1, i;

    for (i = 2; i <= NUM_FIBONACCI; ++i)    {       
        f = f0 + f1;
        f0 = f1;
        f1 = f; 
        // printf("i: %d, f: %d\n", i, f);
    }
}
void triangulo() {
    int t = 0, i;

    for (i = 1; i <= NUM_TRIANGULO; i++) {      
        t += (i-1);
        // printf("i: %d, t: %d\n", i, t);
    }
}


int main (int argc, char** argv) {
    // fibonacci();
    // triangulo();
    return 0;
}