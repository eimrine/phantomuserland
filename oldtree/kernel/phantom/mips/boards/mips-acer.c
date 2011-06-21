/**
 *
 * Phantom OS
 *
 * Copyright (C) 2005-2011 Dmitry Zavalishin, dz@dz.ru
 *
 * MIPS Acer Pica hardware mappings.
 *
**/

#include <kernel/board.h>
#include <kernel/trap.h>
#include <kernel/interrupts.h>
#include <kernel/driver.h>
#include <kernel/stats.h>

#include <hal.h>
#include <assert.h>
#include <stdio.h>
//#include <arm/memio.h>

//#include "arm-icp.h"

#define DEBUG_MSG_PREFIX "board"
#include <debug_ext.h>
#define debug_level_flow 6
#define debug_level_error 10
#define debug_level_info 10

char board_name[] = "Acer Pica";

static char * symtab_getname( void *addr );


static int icp_irq_dispatch(struct trap_state *ts);
static void sched_soft_int( void *a );

void board_init_early(void)
{

    // TODO wrong place - must be in arm arch code
    //phantom_symtab_getname = symtab_getname;
}

void board_init_cpu_management(void)
{
}



void board_init_kernel_timer(void)
{
    //icp_timer0_init(100);
}

void board_start_smp(void)
{
    // I'm single-CPU board, sorry.
}

// -----------------------------------------------------------------------
// Arm -mpoke-function-name
// -----------------------------------------------------------------------

/*
static char * symtab_getname( void *addr )
{
    int len = *(int*)(addr-4);
    if( (len & 0xFF000000) != 0xFF000000 )
        return "?";

    return (char *)(addr - 4 - (len&0xFFFFFF));
}
*/

// -----------------------------------------------------------------------
// Interrupts processing
// -----------------------------------------------------------------------


void board_interrupt_enable(int irq)
{
#warning todo
}

void board_interrupt_disable(int irq)
{
#warning todo
}

void board_init_interrupts(void)
{
    board_interrupts_disable_all();
    //phantom_trap_handlers[T_IRQ] = icp_irq_dispatch;
#warning todo
}

void board_interrupts_disable_all(void)
{
#warning todo
}

// TODO this seems to be pretty arch indep?
static void process_irq(struct trap_state *ts, int irq)
{
#warning todo
/*
    ts->intno = irq;

    board_interrupt_disable(irq);

    irq_nest++;
    call_irq_handler( ts, irq );
    irq_nest--;

    board_interrupt_enable(irq); // TODO Wrong! Int handler might disable itself! Keep local mask.

    STAT_INC_CNT(STAT_CNT_INTERRUPT);

    if(irq_nest)
        return;

    // Now for soft IRQs
    irq_nest = SOFT_IRQ_DISABLED|SOFT_IRQ_NOT_PENDING;
    hal_softirq_dispatcher(ts);
    ENABLE_SOFT_IRQ();
*/
}


static int icp_irq_dispatch(struct trap_state *ts)
{
#warning todo
    //process_irq(ts, nirq);

    return 0; // We're ok
}


void board_sched_cause_soft_irq(void)
{
    phantom_scheduler_soft_interrupt();
}


// -----------------------------------------------------------------------
// Drivers
// -----------------------------------------------------------------------


//phantom_device_t * driver_pl011_uart_probe( int port, int irq, int stage );


// NB! No network drivers on stage 0!
static isa_probe_t board_drivers[] =
{


/*
    { "GPIO", 		driver_mem_icp_gpio_probe, 	0, 0xC9000000, 0 },
    { "LCD", 		driver_mem_icp_lcd_probe, 	0, 0xC0000000, 22 },

    { "touch",		driver_mem_icp_touch_probe,   	1, 0x1E000000, 28 },
    { "PL041.Audio",   	driver_mem_pl041_audio_probe,   2, 0x1D000000, 25 },



    { "LEDS", 		driver_mem_icp_leds_probe, 	0, 0x1A000000, 0 },

    { "LAN91C111", 	driver_mem_LAN91C111_net_probe, 2, 0xC8000000, 27 },
*/

    // End of list marker
    { 0, 0, 0, 0, 0 },
};



void board_make_driver_map(void)
{
    //int id = R32(ICP_IDFIELD);

    //if( (id >> 24) != 0x41 )        SHOW_ERROR( 0, "Board manufacturer is %d, not %d", (id >> 24), 0x41 );

    phantom_register_drivers(board_drivers);
}


// -----------------------------------------------------------------------
// stubs
// -----------------------------------------------------------------------

#warning stubs!
void  paging_device_start_read_rq( void *pdev, void *pager_current_request, void *page_device_io_final_callback )
{
    (void) pdev;
    (void) pager_current_request;
    (void) page_device_io_final_callback;
}

void  paging_device_start_write_rq( void *pdev, void *pager_current_request, void *page_device_io_final_callback )
{
    (void) pdev;
    (void) pager_current_request;
    (void) page_device_io_final_callback;
}

void init_paging_device(void)
{
}

int phantom_dev_keyboard_getc(void)
{
    return debug_console_getc();
}

int phantom_scan_console_getc(void)
{
    return debug_console_getc();
}


int phantom_dev_keyboard_get_key()
{
//#warning completely wrong!!!
    //    return debug_console_getc();
    while(1)
        hal_sleep_msec(10000);
}


int driver_isa_vga_putc(int c )
{
    //debug_console_putc(c);
    return c;
}



void rtc_read_tm() {}

long long arch_get_rtc_delta() { return 0LL; }



void board_fill_memory_map( amap_t *ram_map )
{
#warning todo

    extern char end[];

    int uptokernel = (int)&end;

//    int len = 256*1024*1024;
    //int len = 128*1024*1024;
    //assert( 0 == amap_modify( ram_map, uptokernel, len-uptokernel, MEM_MAP_HI_RAM) );

	//int start = 0x10000000;
	//len =       0xFFFFFFFF-start;
    //assert( 0 == amap_modify( ram_map, start, len, MEM_MAP_DEV_MEM) );
}




