/*
 * Copyright 2015, NICTA
 *
 * This software may be distributed and modified according to the terms of
 * the BSD 2-Clause license. Note that NO WARRANTY is provided.
 * See "LICENSE_BSD2.txt" for details.
 *
 * @TAG(NICTA_BSD)
 */

/*
 * seL4 tutorial part 2: create and run a new thread
 */

/* Include Kconfig variables. */
#include <autoconf.h>

#include <stdio.h>
#include <time.h>
#include <assert.h>

#include <sel4/sel4.h>

#include <simple/simple.h>
#include <simple-default/simple-default.h>

#include <vka/object.h>

#include <allocman/allocman.h>
#include <allocman/bootstrap.h>
#include <allocman/vka.h>

#include <utils/zf_log.h>
#include <sel4utils/sel4_zf_logif.h>

#define debug(fmt, ...) fprintf(stderr, "DEBUG: " fmt "\n", ##__VA_ARGS__)
#define warn(fmt, ...) fprintf(stderr, "WARN: " fmt "\n", ##__VA_ARGS__)
#define fatal(fmt, ...) { fprintf(stderr, "FATAL: " fmt "\n", ##__VA_ARGS__); exit(1); }

/* global environment variables */

/* seL4_BootInfo defined in bootinfo.h
 * Links to source: https://wiki.sel4.systems/seL4%20Tutorial%202#Globals_links:
seL4_BootInfo *info;

simple_t defined in simple.h
 * Links to source: https://wiki.sel4.systems/seL4%20Tutorial%202#Globals_links: */
simple_t simple;

/* vka_t defined in vka.h
 * Links to source: https://wiki.sel4.systems/seL4%20Tutorial%202#Globals_links: */
vka_t vka;

/* allocaman_t defined in allocman.h
 * Links to source: https://wiki.sel4.systems/seL4%20Tutorial%202#Globals_links: */
allocman_t *allocman;

/* static memory for the allocator to bootstrap with */
#define ALLOCATOR_STATIC_POOL_SIZE ((1 << seL4_PageBits) * 10)
static char allocator_mem_pool[ALLOCATOR_STATIC_POOL_SIZE];

/* stack for the new thread */
#define THREAD_2_STACK_SIZE 512
static uint64_t thread_2_stack[THREAD_2_STACK_SIZE];

/* convenience function in util.c:
 * Links to source: https://wiki.sel4.systems/seL4%20Tutorial%202#Globals_links:
 */
extern void name_thread(seL4_CPtr tcb, char *name);

/* function to run in the new thread */
void thread_2(void) {
    /* TODO 15: print something */
    /* hint: printf() */

    debug("Init has given birth to me!");

    /* never exit */
    while(1);
}

int main(void)
{
    int error;

    zf_log_set_tag_prefix("hello-2:");
    name_thread(seL4_CapInitThreadTCB, "hello-2");

    seL4_BootInfo* binfo;
    seL4_CPtr cnode;
    seL4_CPtr pagedir;
    vka_object_t tcb = {0};

    binfo = seL4_GetBootInfo();

    simple_default_init_bootinfo(&simple, binfo);

    simple_print(&simple);

    debug("Bootstrapping allocman...");
    allocman = bootstrap_use_current_simple(&simple, ALLOCATOR_STATIC_POOL_SIZE,
                                             allocator_mem_pool);
    ZF_LOGF_IF(allocman == NULL, "Failed to initialize alloc manager.\n"
        "\tMemory pool sufficiently sized?\n"
        "\tMemory pool pointer valid?\n");
    debug("OK!");

    allocman_make_vka(&vka, allocman);

    cnode = simple_get_cnode(&simple);

    pagedir = simple_get_pd(&simple);

    error = vka_alloc_tcb(&vka, &tcb);
    ZF_LOGF_IFERR(error, "Failed to allocate new TCB.\n"
        "\tVKA given sufficient bootstrap memory?");

    debug("Configuring new TCB...");
    const seL4_PrioProps_t prio = seL4_PrioProps_new(seL4_MaxPrio, seL4_MaxPrio);
    error = seL4_TCB_Configure(tcb.cptr, seL4_CapNull, prio, cnode,
                               seL4_NilData, pagedir, seL4_NilData, 0, 0);
    ZF_LOGF_IFERR(error, "Failed to configure the new TCB object.\n"
        "\tWe're running the new thread with the root thread's CSpace.\n"
        "\tWe're running the new thread in the root thread's VSpace.\n"
        "\tWe will not be executing any IPC in this app.\n");
    debug("OK!");

    name_thread(tcb.cptr, "new_thread");

    seL4_UserContext regs = {0};

    debug("IP: %p", thread_2);
    sel4utils_set_instruction_pointer(&regs, (seL4_Word) thread_2);

    const int stack_alignment_requirement = sizeof(seL4_Word) * 2;
    uintptr_t thread_2_stack_top = (uintptr_t)thread_2_stack + sizeof(thread_2_stack);
    ZF_LOGF_IF(thread_2_stack_top % (stack_alignment_requirement) != 0,
        "Stack top isn't aligned correctly to a %dB boundary.\n"
        "\tDouble check to ensure you're not trampling.",
        stack_alignment_requirement);

    debug("SP: %p", (void *) thread_2_stack_top);

    sel4utils_set_stack_pointer(&regs, thread_2_stack_top);

    error = seL4_TCB_WriteRegisters(tcb.cptr, 0, 0, 2, &regs);
    ZF_LOGF_IFERR(error, "Failed to write the new thread's register set.\n"
        "\tDid you write the correct number of registers? See arg4.\n");

    debug("New thread is starting...");
    error = seL4_TCB_Resume(tcb.cptr);
    ZF_LOGF_IFERR(error, "Failed to start new thread.\n");

    debug("Init is done!");

    return 0;
}
