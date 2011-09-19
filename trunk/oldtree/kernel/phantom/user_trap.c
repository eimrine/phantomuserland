/**
 *
 * Phantom OS
 *
 * Copyright (C) 2005-2010 Dmitry Zavalishin, dz@dz.ru
 *
 * Translate trap to signal. If no one handling - kill the thread.
 *
 *
**/

#include <kernel/trap.h>
#include <kernel/interrupts.h>
#include <phantom_assert.h>
#include <phantom_libc.h>
#include <threads.h>
#include <thread_private.h>
#include "misc.h"

#if ARCH_ia32
#  include <ia32/seg.h>
#endif


//! This is what called from low-level asm trap code
void
phantom_kernel_trap( struct trap_state *ts )
{
    int trapno = ts->trapno;

    check_global_lock_entry_count(); // Debug

    int (*handler)(struct trap_state *ts)
        = phantom_trap_handlers[trapno];

    if(handler == 0)
    {
        phantom_check_user_trap( ts ); // returns only if not an user mode thread
        trap_panic(ts);
    }

    if(handler(ts))
    {
        dump_ss(ts);
        panic("Trap handler failed for trap %s\n", trap_name(trapno));
    }

#if ARCH_arm
    // Why just on arm? Will do any harm on PC? Need to check return value from handler to skip SoftIRQ processing?
    //hal_softirq_dispatcher(ts);
#endif
}



void
phantom_check_user_trap( struct trap_state *ts )
{
    // In interrupt? Surely this is a kernel problem, don't
    // deliver to curr thread!
    if(IN_INTERRUPT())
        return;

    // TODO move to some arch func?
#if ARCH_ia32
    if( ts->cs == KERNEL_CS )
        return;
#endif

    phantom_thread_t *t = GET_CURRENT_THREAD();
    int tid = t->tid;

    if(t->thread_flags & THREAD_FLAG_USER)
    {
        // Try thread trap handler first. If it returns zero - trap
        // is supposed to be handled
        if(t->trap_handler)
        {
            int sig_no = trap2signo(ts);
            if( 0 == ( t->trap_handler( sig_no, ts ) ) )
                return;
        }

        printf("Usermode thread %d killed due to unexpected trap\n", tid);
        t_kill_thread( tid );
        // Will panic below if returned
        printf("Usermode trap panic in thread %d\n", tid);
        trap_panic(ts);
    }
    printf("? trap not from kernel CS, and thread has no THREAD_FLAG_USER, tid %d\n", tid);
    // Not user mode, return
}

int trap_panic(struct trap_state *ts)
{
    dump_ss(ts);
    int type = ts->trapno;

    panic("Unexpected trap %s\n", trap_name(type));

    return -1;
}
