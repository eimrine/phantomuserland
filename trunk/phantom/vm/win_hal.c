/**
 *
 * Phantom OS
 *
 * Copyright (C) 2005-2009 Dmitry Zavalishin, dz@dz.ru
 *
 * Kernel ready: no
 * Preliminary: yes
 *
 * This source file implements Windows based wrapper for VM to
 * run in Windows-hosted environment.
 *
**/

#include <stdarg.h>
//#include <stdlib.h>


#include "event.h"
#include "gcc_replacements.h"
#include "vm/internal_da.h"
#include "vm/alloc.h"
#include "hal.h"
#include "main.h"
#include "vm/alloc.h"
#include "video/win_local.h"


struct hardware_abstraction_level    	hal;

int phantom_is_a_real_kernel() { return 0; }


void        hal_init( vmem_ptr_t va, long vs )
{
	hal_printf("Win32 HAL init\n");

	hal.object_vspace = va;
	hal.object_vsize = vs;

	pvm_alloc_init( va, vs );

	//hal.object_vsize = 40 * 1024 * 1024; // 40 MB

	//hal.object_vspace = (void *)PHANTOM_AMAP_START_VM_POOL;

	//hal_printf("HAL init VM at 0x%X\n", hal.object_vspace);


	//hal_init_vm_map();

}

vmem_ptr_t hal_object_space_address() { return hal.object_vspace; }



void    hal_halt()
{
	//fflush(stderr);
	hal_printf("\n\nhal halt called, exiting.\n");
	getchar();
	exit(1);
}

extern int sleep(int);
void        hal_sleep_msec( int miliseconds )
{
	//usleep(1000*miliseconds);
	sleep( ((miliseconds-1)/1000)+1 );
}





// -----------------------------------------------------------------------
// Spinlocks


#define	_spin_unlock(p) \
	({  register int _u__ ; \
	    __asm__ volatile("xorl %0, %0; \n\
			  xchgl %0, %1" \
			: "=&r" (_u__), "=m" (*(p)) ); \
	    0; })

// ret 1 on success
#define	_spin_try_lock(p)\
	(!({  register int _r__; \
	    __asm__ volatile("movl $1, %0; \n\
			  xchgl %0, %1" \
			: "=&r" (_r__), "=m" (*(p)) ); \
	    _r__; }))


void hal_spin_init(hal_spinlock_t *sl) { sl->lock = 0; }

void hal_spin_lock(hal_spinlock_t *sl)
{
    while( !  _spin_try_lock( &(sl->lock)  ) )
        while( sl->lock )
            ;
}

void hal_spin_unlock(hal_spinlock_t *sl)
{
    _spin_unlock(&(sl->lock));
}

// -----------------------------------------------------------------------

// alloc wants it
int phantom_virtual_machine_threads_stopped = 0;



// -----------------------------------------------------------------------
// TODO - implement those ones


int hal_mutex_init(hal_mutex_t *m, const char *name)
{
    return 0;
}

int hal_mutex_lock(hal_mutex_t *m)
{
    return 0;
}

int hal_mutex_unlock(hal_mutex_t *m)
{
    return 0;
}



int hal_cond_init( hal_cond_t *c, const char *name )
{
    return 0;
}






//int debug_print = 0;




// no snaps here

volatile int phantom_virtual_machine_snap_request = 0;

void phantom_thread_wait_4_snap()
{
    // Just return
}


void phantom_activate_thread()
{
    // Threads do not work in this mode
}



void phantom_snapper_wait_4_threads()
{
    // Do nothing in non-kernel version
    // Must be implemented if no-kernel multithread will be done
}


void phantom_snapper_reenable_threads()
{
    //
}

int phantom_dev_keyboard_getc(void)
{
    return getchar();
}






void phantom_thread_sleep_worker( struct data_area_4_thread *thda )
{
    /*if(phantom_virtual_machine_stop_request)
    {
        if(DEBUG) printf("Thread will die now\n");
        pthread_exit(0);
    }*/


    //phantom_virtual_machine_threads_stopped++;

    while(thda->sleep_flag)
        sleep(1);

    //phantom_virtual_machine_threads_stopped--;

}


void phantom_thread_put_asleep( struct data_area_4_thread *thda )
{
    thda->sleep_flag++;
    // NB! This will work if called from SYS only! That's
    // ok since no other bytecode instr can call this.
    // Real sleep happens in phantom_thread_sleep_worker
}


void phantom_thread_wake_up( struct data_area_4_thread *thda )
{
    thda->sleep_flag--;
}



void phantom_wakeup_after_msec(long msec)
{
    hal_sleep_msec(msec);
}



void panic(const char *fmt, ...)
{
	va_list vl;

	printf("\nPanic: ");
	va_start(vl, fmt);
	vprintf(fmt, vl);
	va_end(vl);

        //save_mem(mem, size);
	printf("\nPress Enter...");

	pvm_memcheck();
	getchar();
	exit(1);
}



static void *dm_mem, *dm_copy;
static int dm_size = 0;
void setDiffMem( void *mem, void *copy, int size )
{
    dm_mem = mem;
    dm_copy = copy;
    dm_size = size;
}

void checkDiffMem()
{
    char *mem = dm_mem;
    char *copy = dm_copy;
    char *start = dm_mem;
    int prevdiff = 0;

return;

    int i = dm_size;
    while( i-- )
    {
        if( *mem != *copy )
        {
            if( !prevdiff )
            {
                printf(", d@ 0x%04x", mem - start );
            }
            prevdiff = prevdiff ? 2 : 1;
            *copy = *mem;
        }
        else
        {
            if( prevdiff == 2 )
            {
                printf( "-%04x", mem - start -1 );
                prevdiff = 0;
            }
        }
        mem++;
        copy++;
    }

    printf(" Press Enter...");
    getchar();
}


void event_q_put_global( ui_event_t *e ) {}


void event_q_put_win( int x, int y, int info, struct drv_video_window *   focus )
{
}

int drv_video_window_get_event( drv_video_window_t *w, struct ui_event *e, int wait )
{
    printf("\nGetEvent!?\n");
    w->events_count--;
    assert(!wait);
    return 0;
}


int hal_save_cli() { return 1; }
void hal_sti() {}
void hal_cli() {}
