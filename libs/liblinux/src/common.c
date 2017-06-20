#include <common.h>

vka_t *vka;

simple_t *simple;

seL4_CPtr pagedir_cap;
seL4_CPtr cspace_cap;

void set_vka(vka_t *v)
{
    vka = vka;
}

void set_simple(simple_t* s)
{
    simple = s;
}

void set_pagedir_cap(seL4_CPtr cp)
{
    pagedir_cap = cp;
}

void set_cspace(seL4_CPtr cp)
{
    cspace_cap = cp;
}
