#include <stdlib.h>
#include <stdio.h>
#include <string.h>

unsigned long _copy_from_user(void *dest, void *src, unsigned long s)
{
    memcpy(dest, src, s);
    return 0;
}

unsigned long _copy_to_user(void *dest, void *src, unsigned long s)
{
    // FIXME: check if write is correct before
    memcpy(dest, src, s);
    return 0;
}
