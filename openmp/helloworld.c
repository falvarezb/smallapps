#include "omp.h"
#include <stdio.h>

int main(int argc, char const *argv[])
{

    #pragma omp parallel
    {
        int ID = omp_get_thread_num();
        printf(" hello(%d) ", ID);
        printf(" world(%d) \n", ID);
    }
}
