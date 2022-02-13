#include <stdlib.h>

int sign(int a)
{
    if (a == 0)
    {
        return 0;
    }

    return a / abs(a);
}
