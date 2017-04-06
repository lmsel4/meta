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

// static memory for the allocator to bootstrap with
#define ALLOCATOR_STATIC_POOL_SIZE ((1 << seL4_PageBits) * 10)
static char allocator_mem_pool[ALLOCATOR_STATIC_POOL_SIZE];

// stack for the new thread
#define THREAD_2_STACK_SIZE 512
static uint64_t thread_2_stack[THREAD_2_STACK_SIZE];

extern void name_thread(seL4_CPtr tcb, char *name);

// module hardcode
extern int  (*hello_init)(void)     asm("__initcall_hello_init6"); // TODO hardcoded
extern void (*hello_exit)(void)     asm("__exitcall_hello_exit"); // TODO hardcoded

int module_handler() {
    int err;

    err = hello_init();
    if (err)
        return err;

    hello_exit();

    return 0;
}

int spawn_new_module(vka_t const* const vka, seL4_CPtr const cspace_cap,
                     seL4_CPtr const pd_cap) {
    int error;

    // create new thread control block (TCB)
    vka_object_t tcb_object = {0};
    error = vka_alloc_tcb(vka, &tcb_object);
    ZF_LOGF_IFERR(error, "Failed to allocate new TCB.\n"
                  "\tVKA given sufficient bootstrap memory?");

    // fill TCB
    error = seL4_TCB_Configure(tcb_object.cptr, seL4_CapNull,
                               seL4_PrioProps_new(seL4_MaxPrio, seL4_MaxPrio),
                               cspace_cap, seL4_NilData, pd_cap, seL4_NilData, 0, 0);
    ZF_LOGF_IFERR(error, "Failed to configure the new TCB object.\n"
                  "\tWe're running the new thread with the root thread's CSpace.\n"
                  "\tWe're running the new thread in the root thread's VSpace.\n"
                  "\tWe will not be executing any IPC in this app.\n");

    // give name to thread
    name_thread(tcb_object.cptr, "module-loader: new-module"); // TODO hardcoded

    // create registers
    seL4_UserContext regs = {0};
    sel4utils_set_instruction_pointer(&regs, (seL4_Word) module_handler); // TODO hardcoded

    // check stack is aligned correctly
    const int stack_alignment_requirement = sizeof(seL4_Word) * 2;
    uintptr_t thread_2_stack_top = (uintptr_t)thread_2_stack + sizeof(thread_2_stack);
    ZF_LOGF_IF(thread_2_stack_top % (stack_alignment_requirement) != 0,
               "Stack top isn't aligned correctly to a %dB boundary.\n"
               "\tDouble check to ensure you're not trampling.",
               stack_alignment_requirement);

    // add stack pointer to registers
    sel4utils_set_stack_pointer(&regs, thread_2_stack_top);

    // write registers to TCB
    error = seL4_TCB_WriteRegisters(tcb_object.cptr, 0, 0, 2, &regs);
    ZF_LOGF_IFERR(error, "Failed to write the new thread's register set.\n"
                  "\tDid you write the correct number of registers? See arg4.\n");

    // start thread
    error = seL4_TCB_Resume(tcb_object.cptr);
    ZF_LOGF_IFERR(error, "Failed to start new thread.\n");

    return error;
}

int main(void)
{
    debug("Init start");

    int error;

    // get boot info
    seL4_BootInfo *info;
    info = seL4_GetBootInfo();
    ZF_LOGF_IF(info == NULL, "Failed to get bootinfo.");

    // set log name
    zf_log_set_tag_prefix("hello-2:");
    name_thread(seL4_CapInitThreadTCB, "hello-2");

    // init simple struct to get info from kernel
    simple_t simple;
    simple_default_init_bootinfo(&simple, info);

    // create memory allocator
    allocman_t *allocman;
    allocman = bootstrap_use_current_simple(&simple, ALLOCATOR_STATIC_POOL_SIZE,
                                            allocator_mem_pool);
    ZF_LOGF_IF(allocman == NULL, "Failed to initialize alloc manager.\n"
               "\tMemory pool sufficiently sized?\n"
               "\tMemory pool pointer valid?\n");

    // create kernel allocator
    vka_t vka;
    allocman_make_vka(&vka, allocman);

    // get init capabilities
    seL4_CPtr cspace_cap;
    cspace_cap = simple_get_cnode(&simple);

    // get init virtual space
    seL4_CPtr pd_cap;
    pd_cap = simple_get_pd(&simple);

    // spawn the module
    spawn_new_module(&vka, cspace_cap, pd_cap);

    // end of init
    debug("Init is done!");

    return 0;
}
