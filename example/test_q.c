#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int ret1 = 0;
    int ret2 = 0;

    // call qooz for duplicate optimizer test.
    asm volatile( "qoox.k qzero,qt1,qzero,1");
    asm volatile( "qoox.k qzero,qt1,qzero,1");
    asm volatile( "qoox.k qzero,qt1,qzero,1");

    // call qooh
    asm volatile( "qooh.k qzero,qt1,qzero,1");
    asm volatile( "qooh.k qzero,qt1,qzero,16");

    // call qmeas
    asm volatile( "qmeas.k  %0,qt1,qzero,0" :"=r"(ret1));
    printf("%d\n", ret1);
    asm volatile( "qmeas.k  %0,qt1,qzero,1" :"=r"(ret2));
    printf("%d\n", ret2);
    
    return 0;
}
