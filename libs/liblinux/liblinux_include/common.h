#ifndef __COMMON_LIBLINUX_H
#define __COMMON_LIBLINUX_H

#include <sel4/sel4.h>

#include <simple/simple.h>
#include <simple-default/simple-default.h>

#include <vka/object.h>

#include <allocman/allocman.h>
#include <allocman/bootstrap.h>
#include <allocman/vka.h>

extern simple_t *simple;

extern vka_t *vka;

extern vspace_t *vspace;

extern void set_simple(simple_t* simple);
extern void set_vka(vka_t *vkae);
extern void set_pagedir_cap(seL4_CPtr pd_cap);

#endif
