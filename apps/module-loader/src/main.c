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

// IPC
#define IPCBUF_FRAME_SIZE_BITS 12 // use a 4K frame for the IPC buffer
#define IPCBUF_VADDR 0x7000000 // arbitrary (but free) address for IPC buffer

extern void name_thread(seL4_CPtr tcb, char *name);

struct seL4_Module module;
extern void init_module_struct();

simple_t simple;

irq_server_t srv;

vka_t vka;

int module_handler() {
    debug("Module handler spawned successfully!");

    debug("Preparing to run module init function...");

    MODULE_LOAD_FCN(&module);

    init_module_struct();

    debug("Module init function found!");

    debug("Running module init function..");

    module.init();

    debug("Ran module init function");

    while(true);

    return 0;
}

irq_server_t make_irq_server(vspace_t *vs, vka_t *vka, seL4_CPtr cspace, simple_t *simple)
{
    irq_server_t srv;
    int error;
    vka_object_t aep;

    vka_alloc_endpoint(vka, &aep);

    error = irq_server_new(vs, vka, cspace, seL4_MaxPrio, simple,
            seL4_CapNull, 0, 256, &srv);

    ZF_LOGF_IF(error != 0, "Unable to create irq server!");

    return srv;
}

int spawn_new_module(vka_t * const vka, seL4_CPtr const cspace_cap,
        seL4_CPtr const pd_cap, simple_t* simple) {
    int error;

    debug("load module");
    MODULE_LOAD_FCN(&module);
    init_module_struct();

    debug("create new thread control block (TCB)");
    vka_object_t tcb_object = {0};
    error = vka_alloc_tcb(vka, &tcb_object);
    ZF_LOGF_IFERR(error, "Failed to allocate new TCB.\n"
            "\tVKA given sufficient bootstrap memory?");

    debug("create IPC");
    vka_object_t ipc_frame_object;
    error = vka_alloc_frame(vka, IPCBUF_FRAME_SIZE_BITS, &ipc_frame_object);
    ZF_LOGF_IFERR(error, "Failed to alloc a frame for the IPC buffer.\n"
            "\tThe frame size is not the number of bytes, but an exponent.\n"
            "\tNB: This frame is not an immediately usable, virtually mapped page.\n");

    seL4_Word ipc_buffer_vaddr = IPCBUF_VADDR;

    error = seL4_ARCH_Page_Map(ipc_frame_object.cptr, pd_cap, ipc_buffer_vaddr,
            seL4_AllRights, seL4_ARCH_Default_VMAttributes);

    vka_object_t pt_object;
    error =  vka_alloc_page_table(vka, &pt_object);
    ZF_LOGF_IFERR(error, "Failed to allocate new page table.\n");

    error = seL4_ARCH_PageTable_Map(pt_object.cptr, pd_cap,
            ipc_buffer_vaddr, seL4_ARCH_Default_VMAttributes);
    ZF_LOGF_IFERR(error, "Failed to map page table into VSpace.\n"
            "\tWe are inserting a new page table into the top-level table.\n"
            "\tPass a capability to the new page table, and not for example, the IPC buffer frame vaddr.\n");

    error = seL4_ARCH_Page_Map(ipc_frame_object.cptr, pd_cap,
            ipc_buffer_vaddr, seL4_AllRights, seL4_ARCH_Default_VMAttributes);
    ZF_LOGF_IFERR(error, "Failed again to map the IPC buffer frame into the VSpace.\n"
            "\t(It's not supposed to fail.)\n"
            "\tPass a capability to the IPC buffer's physical frame.\n"
            "\tRevisit the first seL4_ARCH_Page_Map call above and double-check your arguments.\n");

    seL4_IPCBuffer *ipcbuf = (seL4_IPCBuffer*)ipc_buffer_vaddr;
    ipcbuf->userData = ipc_buffer_vaddr;

    debug("fill TCB");
    error = seL4_TCB_Configure(tcb_object.cptr, seL4_CapNull,
            seL4_PrioProps_new(seL4_MaxPrio, seL4_MaxPrio),
            cspace_cap, seL4_NilData, pd_cap, seL4_NilData, ipc_buffer_vaddr,
            ipc_frame_object.cptr);
    ZF_LOGF_IFERR(error, "Failed to configure the new TCB object.\n"
            "\tWe're running the new thread with the root thread's CSpace.\n"
            "\tWe're running the new thread in the root thread's VSpace.\n"
            "\tWe will not be executing any IPC in this app.\n");

    debug("give name to thread");
    name_thread(tcb_object.cptr, "module-loader: new-module"); // TODO hardcoded

    debug("create registers");
    seL4_UserContext regs = {0};
    sel4utils_set_instruction_pointer(&regs, (seL4_Word) module.init); // TODO hardcoded

    debug("check stack is aligned correctly");
    const int stack_alignment_requirement = sizeof(seL4_Word) * 2;
    uintptr_t thread_2_stack_top = (uintptr_t)thread_2_stack + sizeof(thread_2_stack);
    ZF_LOGF_IF(thread_2_stack_top % (stack_alignment_requirement) != 0,
            "Stack top isn't aligned correctly to a %dB boundary.\n"
            "\tDouble check to ensure you're not trampling.",
            stack_alignment_requirement);

    debug("add stack pointer to registers");
    sel4utils_set_stack_pointer(&regs, thread_2_stack_top);

    debug("write registers to TCB");
    error = seL4_TCB_WriteRegisters(tcb_object.cptr, 0, 0, 2, &regs);
    ZF_LOGF_IFERR(error, "Failed to write the new thread's register set.\n"
            "\tDid you write the correct number of registers? See arg4.\n");

    debug("start thread");
    debug("about to start: %p", module.init);
    error = seL4_TCB_Resume(tcb_object.cptr);
    ZF_LOGF_IFERR(error, "Failed to start new thread.\n");

    debug("Thread has spawned!");

    return error;
}

void test(int *a)
{
    printf("%d\n", *a);
}

void test_spawn(vka_t * const vka, seL4_CPtr const cspace_cap, vspace_t *vspace,
        seL4_CPtr const pd_cap, simple_t* simple)
{
    int err;
    sel4utils_thread_t thread;
    seL4_CapData_t null = {{0}};
    sel4utils_thread_config_t config = {
        .fault_endpoint = seL4_CapNull,
        .priority = seL4_MaxPrio,
        .cspace = cspace_cap,
        .no_ipc_buffer = false,
        .cspace_root_data = null,
        .custom_stack_size = false,
    };

    err = sel4utils_configure_thread_config(vka, vspace, vspace, config, &thread);

    ZF_LOGF_IFERR(err, "Unable to configure new thread");
err = sel4utils_start_thread(&thread, module_handler, NULL, NULL, 1);

    ZF_LOGF_IFERR(err, "Unable to start new thread!");
}

void print_irq(struct irq_data *data)
{
    printf("Got IRQ: %d\n", data->irq);
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

    srv = make_irq_server(&vspace, &vka, seL4_CapInitThreadCNode, &simple);
    ZF_LOGF_IF(srv == NULL, "Unable to create irq server");

    // spawn the module
    //spawn_new_module(&vka, simple_get_cnode(&simple), seL4_CapInitThreadPD, &simple);
    //test_spawn(&vka, seL4_CapInitThreadCNode, &vspace, seL4_CapInitThreadPD, &simple);
    module_handler();

    // end of init
    debug("Init is done!");

    while(true);

    return 0;
}
