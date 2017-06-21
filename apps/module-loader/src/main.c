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
#include <sel4utils/util.h>
#include <sel4utils/thread.h>
#include <sel4utils/vspace.h>
#include <sel4utils/irq_server.h>
#include <sel4utils/sel4_arch/vspace.h>

#include <loader.h>
#include <common.h>

// static memory for the allocator to bootstrap with
#define ALLOCATOR_STATIC_POOL_SIZE ((1 << seL4_PageBits) * 10)
static char allocator_mem_pool[ALLOCATOR_STATIC_POOL_SIZE];

// stack for the new thread
#define THREAD_2_STACK_SIZE 512
static uint64_t thread_2_stack[THREAD_2_STACK_SIZE];

extern void name_thread(seL4_CPtr tcb, char *name);

struct seL4_Module module;

irq_server_t srv;

int module_handler() {
    debug("Module handler spawned successfully!");

    debug("Preparing to run module init function...");

    MODULE_LOAD_FCN(&module);

    debug("Module init function found!");

    debug("Running module init function..");

    module.init();

    debug("Ran module init function");

    while(true);

    return 0;
}

void test(int *a)
{
    debug("Param is %d", *a);
    while(true);
}

irq_server_t make_irq_server(vspace_t *vs, vka_t *vka, seL4_CPtr cspace, simple_t *simple)
{
    irq_server_t srv;
    int error;
    vka_object_t aep;

    vka_alloc_endpoint(vka, &aep);

    error = irq_server_new(vs, vka, cspace, seL4_MaxPrio, simple,
                           aep.cptr, 0, 256, &srv);

    ZF_LOGF_IF(error != 0, "Unable to create irq server!");

    return srv;
}

int spawn_new_module(vka_t * const vka, seL4_CPtr const cspace_cap,
                     seL4_CPtr const pd_cap, simple_t* simple) {
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

    debug("Thread has spawned!");

    return error;
}

void test_spawn(vka_t * const vka, seL4_CPtr const cspace_cap, vspace_t *vspace,
                seL4_CPtr const pd_cap, simple_t* simple)
{
    int err;
    sel4utils_thread_t thread;
    seL4_CapData_t null = {{0}};

    int a = 5;

    err = sel4utils_configure_thread(vka, vspace, vspace, seL4_CapNull, seL4_MaxPrio,
                               cspace_cap, null, &thread);

    ZF_LOGF_IFERR(err, "Unable to configure new thread");

    err = sel4utils_start_thread(&thread, test, &a, NULL, 1);

    ZF_LOGF_IFERR(err, "Unable to start new thread!");
}

int main(void)
{
    UNUSED int error;
    vspace_t vspace;
    sel4utils_alloc_data_t data;
    allocman_t *allocman;
    seL4_BootInfo *info;

    /* get boot info */
    info = seL4_GetBootInfo();
    ZF_LOGF_IF(info == NULL, "Failed to get bootinfo.");

    /* Set up logging and give us a name: useful for debugging if the thread faults */
    zf_log_set_tag_prefix("hello-4:");
    name_thread(seL4_CapInitThreadTCB, "hello-4");

    /* init simple */
    simple_default_init_bootinfo(&simple, info);

    /* create an allocator */
    allocman = bootstrap_use_current_simple(&simple, ALLOCATOR_STATIC_POOL_SIZE,
                                            allocator_mem_pool);
    ZF_LOGF_IF(allocman == NULL, "Failed to initialize allocator.\n"
               "\tMemory pool sufficiently sized?\n"
               "\tMemory pool pointer valid?\n");

    /* create a vka (interface for interacting with the underlying allocator) */
    allocman_make_vka(&vka, allocman);

    debug("bootstraping vspace..");

    error = sel4utils_bootstrap_vspace_with_bootinfo_leaky(&vspace,
                                                           &data,
                                                           seL4_CapInitThreadPD,
                                                           &vka, info);

    ZF_LOGF_IFERR(error, "Unable to boostrap vspace!");

    set_vka(&vka);

    // spawn the module
    //spawn_new_module(&vka, cspace_cap, pd_cap, simple);
    test_spawn(&vka, seL4_CapInitThreadCNode, &vspace, seL4_CapInitThreadPD, simple);

    srv = make_irq_server(&vspace, &vka, seL4_CapInitThreadCNode, &simple);

    ZF_LOGF_IF(srv == NULL, "Unable to create irq server");

    module_handler();

    // end of init
    debug("Init is done!");

    while(true);

    return 0;
}
