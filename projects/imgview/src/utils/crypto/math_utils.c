#include "math_utils.h"

int gcd(int a, int b)
{
    int rest = 0;

    while(b != 0)
    {
        rest = a % b;
        a = b;
        b = rest;
    }

    return a;
}
