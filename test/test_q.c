#include <stdio.h>
#include <stdlib.h>

int calc() {
    int ret = 0;
    // call qooh
    asm volatile(
    "qooh.k  qa0,qt1,qzero,1"
    );

    // call qmeas
    asm volatile(
    "qmeas.k  %0,qt1,qzero,1"
    :"=r"(ret)
    :
    );
    return ret;
}