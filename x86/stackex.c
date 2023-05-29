#include <stdio.h>

long utilfunc(long a, long b, long c)
{
    long xx = a + 2;
    long yy = b + 3;
    long zz = c + 4;
    long sum = xx + yy + zz;
    // long* p = (long*)0x7ff7bfefd5f0;
    // long* afterp = p+1;
    // long* myxx = &xx;
    // long* myyy = &yy;
    //long pval = *p;

    return xx * yy * zz + sum;
}

long myfunc(long a, long b, long c, long d,
            long e, long f, long g, long h)
{
    //long* myg = 0x7ff7bfefd670;    
    // long* myg = &g;
    // long* myh = &h;    
    // long* return_address = myg-1;
    // long* rbp = myg-2;
    long xx = a * b * c * d * e * f * g * h;
    long yy = a + b + c + d + e + f + g + h;
    long zz = utilfunc(xx, yy, xx % yy);
    // long* myxx = &xx;
    // long* myyy = &yy;
    // long* myzz = &zz;
    return zz + 20;
}

int main(int argc, char const *argv[])
{
    long result = myfunc(1,2,3,4,5,6,7,8);
    printf("result=5%ld", result);
    return 0;
}
