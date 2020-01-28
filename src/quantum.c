#include <stdint.h>
uint64_t qfunc() {
    int ret = 0;
    #ifdef QUANTUM
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
    #else
    printf("qfunc call\n");
    #endif
    return ret;
}
