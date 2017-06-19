#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void _copy_from_user(void *dest, void *src, size_t s)
{
    memcpy(dest, src, s);
}

void _copy_to_user(void *dest, void *src, size_t s)
{
    memcpy(dest, src, s);
}
