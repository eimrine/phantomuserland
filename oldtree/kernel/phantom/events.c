/**
 *
 * Phantom OS
 *
 * Copyright (C) 2005-2010 Dmitry Zavalishin, dz@dz.ru
 *
 * Main win/ui event management code.
 *
 *
**/

#define EVENTS_ENABLED 1
#define KEY_EVENTS 1

// Debug only!
#define DIRECT_DRIVE 0

#define DELIVER2THREAD 1


#define DEBUG_MSG_PREFIX "events"
#include <debug_ext.h>
#define debug_level_flow 0
#define debug_level_error 10
#define debug_level_info 10

#include <phantom_libc.h>
#include <kernel/config.h>

#include <threads.h>
#include <event.h>
#include <dev/key_event.h>
#include <queue.h>
#include <malloc.h>
#include <hal.h>
#include <string.h>
#include <video.h>
#include <console.h>
#include <wtty.h>
#include <time.h>

void drv_video_window_explode_event(struct ui_event *e);
int drv_video_window_get_event( drv_video_window_t *w, struct ui_event *e, int wait );


static void event_push_thread(void);
static void keyboard_read_thread(void);

static int phantom_window_getc(void);

static void allocate_event();
static void push_event( struct ui_event *e );

//static int phantom_window_getc(void);


static int                      event_engine_active = 0;

static hal_mutex_t             main_q_mutex;
static hal_mutex_t             unused_q_mutex;
static hal_cond_t              have_event;


// Each main Q event is on one of these qs
static queue_head_t     unused_events;  // list of unused event structs
static queue_head_t     main_event_q;  	// list of generated events
static int		events_in_q = 0;



int get_n_events_in_q() { return events_in_q; }

#if DELIVER2THREAD
static void w_event_deliver_thread(void);
#endif


#if 1
#define MIN_EVENT_POOL  128
#define MAX_EVENT_POOL  512

//#define MAX_EVENT_POOL  5120
#else

// FIXED: Dies instantly - Panic: recursive mutex lock in count_unused/hal_mutex_lock
#define MIN_EVENT_POOL  1
#define MAX_EVENT_POOL  3
#endif

/**
 *
 * Init UI event queue.
 *
**/

void init_main_event_q()
{
    queue_init(&unused_events);
    queue_init(&main_event_q);

    hal_mutex_init( &main_q_mutex, "Main EvQ" );
    hal_mutex_init( &unused_q_mutex, "Free EvQ" );

    hal_cond_init( &have_event, "UIEvent" );

    hal_mutex_lock( &unused_q_mutex );
    int i = MIN_EVENT_POOL;
    while(i--)
        allocate_event();
    hal_mutex_unlock( &unused_q_mutex );

#if EVENTS_ENABLED
    hal_start_kernel_thread( event_push_thread );


    event_engine_active = 1;

#if KEY_EVENTS
    phantom_set_console_getchar( phantom_window_getc );
    hal_start_kernel_thread( keyboard_read_thread );
#endif
#endif

#if DELIVER2THREAD
    hal_start_kernel_thread( w_event_deliver_thread );
#endif

}


#if KEY_EVENTS

#include <thread_private.h>

static int phantom_window_getc(void)
{
    //SHOW_FLOW0( 11, "window getc" );
    //wtty_t *tty = &(GET_CURRENT_THREAD()->ctty);
    wtty_t *tty = GET_CURRENT_THREAD()->ctty;

    if(tty == 0)
    {
        SHOW_ERROR0( 0, "No wtty, phantom_window_getc loops forever" );
        while(1)
            hal_sleep_msec(10000);
    }

    return wtty_getc( tty );
}
#endif




//! Allocate one more event
static void allocate_event()
{
    struct ui_event *e = calloc( 1, sizeof(struct ui_event) );
    assert(e);
    queue_enter(&unused_events, e, struct ui_event *, echain);
}


//! Get unused event from unused events queue, or allocate new one.
static struct ui_event * get_unused()
{
    struct ui_event *e;

    hal_mutex_lock( &unused_q_mutex );
    if(queue_empty(&unused_events))
    {
        allocate_event();
    }

    if(queue_empty(&unused_events))
        panic("out of events");

    queue_remove_first(&unused_events, e, struct ui_event *, echain);
    hal_mutex_unlock( &unused_q_mutex );
    return e;
}

//! Return (and clear!) unused event to unused q
void return_unused_event(struct ui_event *e)
{
    assert(event_engine_active);
    memset( e, sizeof(struct ui_event), 0 );
    hal_mutex_lock( &unused_q_mutex );
    queue_enter(&unused_events, e, struct ui_event *, echain);
    hal_mutex_unlock( &unused_q_mutex );
}


//! Put filled event onto the main event q
static void put_event(struct ui_event *e)
{
    if(!event_engine_active) return; // Just ignore

    SHOW_FLOW(8, "%p", e);
    hal_mutex_lock( &main_q_mutex );
    events_in_q++;
    queue_enter(&main_event_q, e, struct ui_event *, echain);
    hal_cond_broadcast( &have_event );
    hal_mutex_unlock( &main_q_mutex );
}


//! Count events on unused Q
static int count_unused()
{
    int count = 0;

    hal_mutex_lock( &unused_q_mutex );

    if(queue_empty(&unused_events))
    {
        hal_mutex_unlock( &unused_q_mutex );
        return 0;
    }

    struct ui_event *e;
    queue_iterate(&unused_events, e, struct ui_event *, echain)
    {
        count++;
    }
    hal_mutex_unlock( &unused_q_mutex );

    return count;
}

//! Remove unused events if there are too many
static void remove_extra_unused()
{
    int c = count_unused();
    if(c > MAX_EVENT_POOL)
    {
        struct ui_event *e = get_unused();
        free(e);
    }
}


//! Put (copy of) any event onto the main e q
void event_q_put_e( struct ui_event *in )
{
    struct ui_event *e = get_unused();
    *e = *in;
    put_event(e);
}




//! This thread delivers events from main Q to windows
static void event_push_thread()
{
    hal_set_thread_name("UIEventQ");
    // +1 so that it is a bit higher than regular sys threads
    hal_set_current_thread_priority(PHANTOM_SYS_THREAD_PRIO+1);

#if EVENTS_ENABLED && 1
    while(1)
    {
        remove_extra_unused();

        struct ui_event *e;

        hal_mutex_lock( &main_q_mutex );
        while(queue_empty(&main_event_q))
            hal_cond_wait( &have_event, &main_q_mutex );

        if(queue_empty(&main_event_q))
            panic("out of events");

        queue_remove_first(&main_event_q, e, struct ui_event *, echain);
        events_in_q--;
        hal_mutex_unlock( &main_q_mutex );

        SHOW_FLOW(8, "%p", e);

        // Deliver to 'em
        push_event(e);

        // window code will return when done
        //return_unused_event(e);
    }
#else
    while(1)
    {
    	hal_sleep_msec(20000);
    }
#endif


}

void drv_video_window_receive_event(struct ui_event *e);


static void push_event( struct ui_event *e )
{
    if( e->type == UI_EVENT_TYPE_GLOBAL )
        drv_video_window_explode_event(e);
    else
        drv_video_window_receive_event(e);
}




//! Put key event onto the main e q
void event_q_put_key( int vkey, int ch, int modifiers )
{
    if(!event_engine_active) return; // Just ignore

    struct ui_event *e = get_unused();
    e->type = UI_EVENT_TYPE_KEY;
    e->time = fast_time();
    e->focus= 0;

    e->k.vk = vkey;
    e->k.ch = ch;

    e->modifiers = modifiers;

    put_event(e);
}

/*
//! Put mouse event onto the main e q
void event_q_put_mouse( int x, int y, int buttons )
{
    if(!event_engine_active) return; // Just ignore

    struct ui_event *e = get_unused();
    e->type = UI_EVENT_TYPE_MOUSE;
    e->time = fast_time();
    e->focus= 0;

    e->m.buttons = buttons;
    e->abs_x = x;
    e->abs_y = y;

    put_event(e);
}
*/

//! Put window event onto the main e q
void event_q_put_win( int x, int y, int info, struct drv_video_window *   focus )
{
    if(!event_engine_active) return; // Just ignore

    struct ui_event *e = get_unused();
    e->type = UI_EVENT_TYPE_WIN;
    e->time = fast_time();
    e->focus= focus;

    e->w.info = info;
    e->abs_x = x;
    e->abs_y = y;

    put_event(e);
}



//! Put global event onto the main e q
void event_q_put_global( ui_event_t *ie )
{
    if(!event_engine_active) return; // Just ignore

    struct ui_event *e = get_unused();

    *e = *ie;

    e->type = UI_EVENT_TYPE_GLOBAL;
    e->time = fast_time();
    e->focus= 0;

    put_event(e);
}




// -----------------------------------------------------------------
// Temporarily moved from vm/video/ here
// -----------------------------------------------------------------

#include <drv_video_screen.h>

extern queue_head_t     	allwindows;
extern drv_video_window_t *	focused_window;


//! Select target window
static void select_event_target(struct ui_event *e)
{
    // Don't even try to select destination for this type of event
    if( e->type == UI_EVENT_TYPE_WIN )
        return;

    drv_video_window_t *w;

    e->focus = focused_window;

    // If focused window is being dragged - don't try to change focus - TODO global push_focus_lock/push_focus_lock?
    if( (focused_window != 0) && (focused_window->state & WSTATE_WIN_DRAGGED) )
        return;

    // Keys are delivered to focused window only
    if( e->type == UI_EVENT_TYPE_KEY )
        return;

    // Mouse can be tied to focused window
    //if( focused_window != 0 && focused_window->mouse_tie )
    //    return;

    assert( e->type == UI_EVENT_TYPE_MOUSE );

    int wz = 0;
    queue_iterate(&allwindows, w, drv_video_window_t *, chain)
    {
        if( w->flags & WFLAG_WIN_NOFOCUS )
            continue;
        if( w->z < wz )
            continue;

        wz = w->z;

        if( point_in_win( e->abs_x, e->abs_y, w ) )
        {
            e->focus = w;
            //break; // need to check all to make sure we selected topmost
        }

    }

}


static void w_do_deliver_event(drv_video_window_t *w)
{
    if(w != 0 && w->inKernelEventProcess)
    {
        struct ui_event e;
        int got = drv_video_window_get_event( w, &e, 0 );

        while(got)
        {
            struct ui_event e2;
            if( drv_video_window_get_event( w, &e2, 0 ) )
            {
                // 2 repaints follow
                if((e.type == e2.type) && (e.w.info == e2.w.info) && (e.focus == e2.focus))
                {
                    if((e.w.info == UI_EVENT_WIN_REPAINT) || (e.w.info == UI_EVENT_WIN_REDECORATE))
                    {
                        SHOW_FLOW0( 1, "combined repaint" );
                        // Choose more powerful spell
                        //e.w.info = UI_EVENT_WIN_REDECORATE;
                        // Eat one
                        //e = e2;
                        continue;
                    }
                }
            }
            else
                got = 0;

            SHOW_FLOW(8, "%p, w=%p, us=%p", &e, e.focus, w);

            w->inKernelEventProcess(w, &e);
            e = e2;
        }
    }
}

#if DELIVER2THREAD

static hal_sem_t we_sem;

static void w_event_deliver_thread(void)
{
    hal_sem_init( &we_sem, "wevent" );

    hal_set_thread_name("WEvent");
    hal_set_current_thread_priority(PHANTOM_SYS_THREAD_PRIO+1);

    while(1)
    {
        hal_sem_acquire( &we_sem ); // TODO need some 'acquire_all' method to eat all releases

    restart:
        w_lock();

        drv_video_window_t *w;

        queue_iterate_back(&allwindows, w, drv_video_window_t *, chain)
        {
            if( w->events_count )
            {
                w_unlock();
                w_do_deliver_event(w);
                goto restart;
            }
        }

        w_unlock();
    }
}

#endif


//! Select target and put event to window queue.
void drv_video_window_receive_event(struct ui_event *e)
{
    assert(e);
    drv_video_window_t *w = 0;

    w_lock();

    select_event_target(e);

    drv_video_window_t *later_lost = 0;
    drv_video_window_t *later_gain = 0;

    if( e->focus == 0 )
    {
        //printf("unfocused event");
        goto ret;
    }

    int later_x, later_y;

    // For now use any mouse event to change focus
    if(e->type == UI_EVENT_TYPE_MOUSE && focused_window != e->focus)
    {
        later_x = e->abs_x;
        later_y = e->abs_y;
        if(focused_window != 0)
            later_lost = focused_window;
        later_gain = e->focus;
        focused_window = e->focus;
    }

    // Target
    w = e->focus;

    // Calc relative x, y
    e->rel_x = e->abs_x - w->x;
    e->rel_y = e->abs_y - w->y;

    e->abs_z = w->z;
    e->rel_z = 0;

    if( w->events_count < MAX_WINDOW_EVENTS )
    {
    	SHOW_FLOW(8, "e %p -> w %p", e, w);

#if DIRECT_DRIVE
        if(w != 0 && w->inKernelEventProcess)
            w->inKernelEventProcess(w, e);
#else
    	queue_enter(&(w->events), e, struct ui_event *, echain);
        w->events_count++;
        w->stall = 0;
#endif
    }
    else
        w->stall = 1;

ret:
    w_unlock();

    // It has mutex and can't be called in spinlock
    if(later_lost) event_q_put_win( later_x, later_y, UI_EVENT_WIN_LOST_FOCUS, later_lost );
    if(later_gain) event_q_put_win( later_x, later_y, UI_EVENT_WIN_GOT_FOCUS, later_gain );

#if (!DIRECT_DRIVE) && (!DELIVER2THREAD)
    // Has no own event process thread, serve from here
    w_do_deliver_event(w);
#endif

#if DELIVER2THREAD
    hal_sem_release( &we_sem );
#endif

}


void drv_video_window_explode_event(struct ui_event *e)
{
    drv_video_window_t *w;

#if 1
    if( e->w.info == UI_EVENT_GLOBAL_REPAINT_RECT )
    {
        w_request_async_repaint( &(e->w.rect) );
        return;
    }

#endif

    w_lock();
    queue_iterate(&allwindows, w, drv_video_window_t *, chain)
    {
        rect_t  wr, isect;
        drv_video_window_get_bounds( w, &wr );


        if( e->w.info == UI_EVENT_GLOBAL_REPAINT_RECT )
        {
            int intersects = rect_mul( &isect, &wr, &(e->w.rect) );
            if(intersects)
                event_q_put_win( 0, 0, UI_EVENT_WIN_REPAINT, w );
        }

    }
    w_unlock();

}


//#define DIRECT_DRIVE 1


//! Get next event for this window
int drv_video_window_get_event( drv_video_window_t *w, struct ui_event *e, int wait )
{
    //int ie = 1;
    int ret;
    struct ui_event *tmp;

    // Possibly will have problem when killing window
    while(wait)
    {
        while( w->events_count <= 0 )
            hal_sleep_msec( 100 );

        w_lock();
        if( w->events_count > 0 )
            goto locked;

        w_unlock();
    }

    w_lock();
locked:
    if( w->events_count > 0 )
    {
    	assert(!queue_empty(&(w->events)));
        queue_remove_first(&(w->events), tmp, struct ui_event *, echain);
        w->events_count--;
        ret = 1;
    }
    else
        ret = 0;

    w_unlock();

    if( ret )
    {
    	SHOW_FLOW(8, "tmp %p, w=%p", tmp, tmp->focus);

        *e = *tmp;

        SHOW_FLOW(8, "e %p, w=%p", e, e->focus);

        // Bring it back to main Q engine
        return_unused_event(tmp);
    }

    return ret;
}





static void send_event_to_q(_key_event *event)
{

    static int shifts;

    int dn = event->modifiers & KEY_MODIFIER_DOWN;

    if( event->modifiers & KEY_MODIFIER_UP )
        shifts |= UI_MODIFIER_KEYUP;

    if( dn )
        shifts &= ~UI_MODIFIER_KEYUP;

    switch(event->keycode)
    {
    case KEY_LSHIFT:
        shifts &= ~UI_MODIFIER_LSHIFT;
        if(dn)
            shifts |= UI_MODIFIER_LSHIFT;

    set_comon_shift:
        shifts &= ~UI_MODIFIER_SHIFT;
        if( (shifts & UI_MODIFIER_LSHIFT) || (shifts & UI_MODIFIER_RSHIFT) )
            shifts |= UI_MODIFIER_SHIFT;
        break;

    case KEY_RSHIFT:
        shifts &= ~UI_MODIFIER_RSHIFT;
        if(dn)
            shifts |= UI_MODIFIER_RSHIFT;

        goto set_comon_shift;



    case KEY_LCTRL:
        shifts &= ~UI_MODIFIER_LCTRL;
        if(dn)
            shifts |= UI_MODIFIER_LCTRL;

    set_comon_ctrl:
        shifts &= ~UI_MODIFIER_CTRL;
        if( (shifts & UI_MODIFIER_LCTRL) || (shifts & UI_MODIFIER_RCTRL ) )
            shifts |= UI_MODIFIER_CTRL;
        break;

    case KEY_RCTRL:
        shifts &= ~UI_MODIFIER_RCTRL;
        if(dn)
            shifts |= UI_MODIFIER_RCTRL;

        goto set_comon_ctrl;



    case KEY_LALT:
        shifts &= ~UI_MODIFIER_LALT;
        if(dn)
            shifts |= UI_MODIFIER_LALT;

    set_comon_alt:
        shifts &= ~UI_MODIFIER_ALT;
        if( (shifts & UI_MODIFIER_LALT) || (shifts & UI_MODIFIER_RALT ) )
            shifts |= UI_MODIFIER_ALT;
        break;

    case KEY_RALT:
        shifts &= ~UI_MODIFIER_RALT;
        if(dn)
            shifts |= UI_MODIFIER_RALT;

        goto set_comon_alt;


    case KEY_LWIN:
        if(dn)              shifts |= UI_MODIFIER_LWIN;
        else             	shifts &= ~UI_MODIFIER_LWIN;

    set_comon_win:
        if( (shifts & UI_MODIFIER_LWIN) || (shifts & UI_MODIFIER_RWIN ) )
            shifts |= UI_MODIFIER_WIN;
        else
            shifts &= ~UI_MODIFIER_WIN;
        break;

    case KEY_RWIN:
        if(dn)              shifts |= UI_MODIFIER_RWIN;
        else                shifts &= ~UI_MODIFIER_RWIN;
        goto set_comon_win;




    case KEY_CAPSLOCK:
        if(dn)
        {
            if(shifts & UI_MODIFIER_CAPSLOCK)
                shifts &= ~UI_MODIFIER_CAPSLOCK;
            else
                shifts |= UI_MODIFIER_CAPSLOCK;
        }

    case KEY_SCRLOCK:
        if(dn)
        {
            if(shifts & UI_MODIFIER_SCRLOCK)
                shifts &= ~UI_MODIFIER_SCRLOCK;
            else
                shifts |= UI_MODIFIER_SCRLOCK;
        }

    case KEY_PAD_NUMLOCK:
        if(dn)
        {
            if(shifts & UI_MODIFIER_NUMLOCK)
                shifts &= ~UI_MODIFIER_NUMLOCK;
            else
                shifts |= UI_MODIFIER_NUMLOCK;
        }

    }

    event_q_put_key( event->keycode, event->keychar, shifts );

}






static void keyboard_read_thread(void)
{
    hal_set_thread_name("KeyEvents");
    hal_set_current_thread_priority(PHANTOM_SYS_THREAD_PRIO);

    while(1)
    {
        _key_event ke;

        phantom_dev_keyboard_get_key( &ke );
        send_event_to_q(&ke);
    }
}



static void wtty_wrap(wtty_t *w)
{
    if( w->getpos >= WTTY_BUFSIZE )        w->getpos = 0;
    if( w->putpos >= WTTY_BUFSIZE )        w->putpos = 0;
}


int wtty_getc(wtty_t *w)
{
    int ret;

    SHOW_FLOW( 11, "wtty getc %p", w );

    hal_mutex_lock(&w->mutex);

    while(w->getpos == w->putpos)
        hal_cond_wait( &w->cond, &w->mutex );

    wtty_wrap(w);

    ret = w->buf[w->getpos++];

    hal_mutex_unlock(&w->mutex);

    return ret;
}

int wtty_is_empty(wtty_t *w)
{
    return (w->getpos == w->putpos);
}


/*
void wtty_putc(wtty_t *w, int c)
{
    hal_mutex_lock(&w->mutex);
    wtty_wrap(w);

    hal_mutex_unlock(&w->mutex);
}
*/

errno_t wtty_putc_nowait(wtty_t *w, int c)
{
    int ret = 0;
    hal_mutex_lock(&w->mutex);
    wtty_wrap(w);

    SHOW_FLOW( 11, "wtty putc %p", w );

    if( (w->putpos+1 == w->getpos) || ( w->putpos+1 >= WTTY_BUFSIZE && w->getpos == 0 ) )
    {
        SHOW_ERROR0( 10, "wtty putc fail" );
        ret = ENOMEM;
    }
    else
    {
        w->buf[w->putpos++] = c;
        hal_cond_signal( &w->cond );
    }

    hal_mutex_unlock(&w->mutex);
    return ret;
}


wtty_t * wtty_init(void)
{
    wtty_t *w = calloc( 1, sizeof(wtty_t) );
    assert(w);

    //w->getpos = 0;
    //w->putpos = 0;
    hal_mutex_init( &w->mutex, "wtty" );
    hal_cond_init( &w->cond, "wtty" );

    return w;
}




