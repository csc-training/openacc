#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _OPENACC
#include <openacc.h>
#endif

#define NX 102400

int main(void)
{
    double vecA[NX], vecB[NX];
    double sum, psum;
    int i;

    /* Initialization of the vectors */
    for (i = 0; i < NX; i++) {
        vecA[i] = 1.0 / ((double) (NX - i));
        vecB[i] = vecA[i] * vecA[i];
    }

    sum = 0.0;
    for (i = 0; i < NX; i++) {
        sum += vecA[i] * vecB[i];
    }
    printf("Sum on host: %18.16f\n", sum);

    sum = 0.0;
    /* Dot product using reduction */
#pragma acc parallel loop copyin(vecA[0:NX], vecB[0:NX]) reduction(+:sum)
    for (i = 0; i < NX; i++) {
        sum += vecA[i] * vecB[i];
    }
    printf("Sum using OpenACC reduction: %18.16f\n", sum);

    return 0;
}
