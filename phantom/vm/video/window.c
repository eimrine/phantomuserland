/**
 *
 * Phantom OS
 *
 * Copyright (C) 2005-2009 Dmitry Zavalishin, dz@dz.ru
 *
 * Windowing system internals and housekeeping.
 *
 *
**/

#include <drv_video_screen.h>
#include <assert.h>
#include <phantom_libc.h>
#include <event.h>
#include <spinlock.h>

#include "hal.h"
#include "win_local.h"

//static void defaultEventProcessor();


//static
queue_head_t     	allwindows = { &allwindows, &allwindows };

//static
drv_video_window_t *	focused_window = 0;

//static
hal_spinlock_t  		allw_lock;

// here we have a problem - some windows are persistent and
// keep their generation. So on start we need to advance
// generation twice to make sure all windows are catched up
//static int win_generation = 0;







void drv_video_window_free(drv_video_window_t *w)
{
    drv_video_window_destroy(w);
    free(w);
}



static void
common_window_init( drv_video_window_t *w,
                        int xsize, int ysize )
{
    w->xsize = xsize;
    w->ysize = ysize;

    w->li = w->ti = w->ri = w->bi = 0;

    //w->generation = 0;

    w->x = 0;
    w->y = 0;
    w->z = 0xFE; // quite atop

    w->bg = COLOR_BLACK;

    queue_init(&(w->events));
    w->events_count = 0;
    w->stall = 0;

    w->inKernelEventProcess = defaultWindowEventProcessor;
    w->owner = 0;
}



drv_video_window_t *private_drv_video_window_create(int xsize, int ysize)
{
    drv_video_window_t *w = calloc(1,drv_video_window_bytes(xsize,ysize));
    if(w == 0)
        return 0;

    //w->xsize = xsize;
    //w->ysize = ysize;
    //w->z = 0xFE; // quite atop

    //w->li = w->ti = w->ri = w->bi = 0;

    //queue_init(&(w->events));
    //w->events_count = 0;
    //w->stall = 0;

    common_window_init( w, xsize, ysize );

    return w;
}



drv_video_window_t *
drv_video_window_create(
                        int xsize, int ysize,
                        int x, int y,
                        rgba_t bg, const char *title )
{
    drv_video_window_t *w = private_drv_video_window_create(xsize, ysize);
    drv_video_window_init( w, xsize, ysize, x, y, bg );
    w->title = title;
    return w;
}



// for statically allocated ones
void
drv_video_window_init( drv_video_window_t *w,
                        int xsize, int ysize,
                        int x, int y,
                        rgba_t bg )
{
    common_window_init( w, xsize, ysize );

    //w->xsize = xsize;
    //w->ysize = ysize;

    //w->li = w->ti = w->ri = w->bi = 0;

    w->flags = WFLAG_WIN_DECORATED;
    //w->generation = 0;

    w->x = x;
    w->y = y;
    //w->z = 0xFE; // quite atop
    w->bg = bg;

    w->title = "?";

    //queue_init(&(w->events));
    //w->events_count = 0;
    //w->stall = 0;

    win_make_decorations(w);

    /*
    int ie = hal_save_cli();
    hal_spin_lock( &allw_lock );
    queue_enter(&allwindows, w, drv_video_window_t *, chain);
    hal_spin_unlock( &allw_lock );
    if(ie) hal_sti();
    */

    drv_video_window_enter_allwq(w);

}


void drv_video_window_enter_allwq( drv_video_window_t *w)
{
    int ie = hal_save_cli();
    hal_spin_lock( &allw_lock );
    queue_enter(&allwindows, w, drv_video_window_t *, chain);
    hal_spin_unlock( &allw_lock );
    if(ie) hal_sti();
}


void drv_video_window_destroy(drv_video_window_t *w)
{
    if( focused_window == w )
    {
        event_q_put_win( w->x, w->y, UI_EVENT_WIN_LOST_FOCUS, focused_window );
        focused_window = 0;
    }

    if(!(w->flags & WFLAG_WIN_NOTINALL))
    {
        int ie = hal_save_cli();
        hal_spin_lock( &allw_lock );
        queue_remove(&allwindows, w, drv_video_window_t *, chain);
        hal_spin_unlock( &allw_lock );
        if(ie) hal_sti();
    }
    // This will inform win on its death and unlock event read loop to make
    // sure it doesnt loop in dead window
    event_q_put_win( w->x, w->y, UI_EVENT_WIN_DESTROYED, focused_window );



    // drain event queue
    if( w->events_count > 0 )
    {
        struct ui_event e;

        // Wait to make sure regular event pump drv_video_window_get_event is done
        hal_sleep_msec( 200 ); 

        while( w->events_count > 0 )
            drv_video_window_get_event( w, &e, 0 );
    }

}


void drv_video_window_repaint_all(void)
{
    //win_generation++;
    // redraw all here, or ask some thread to do that
    drv_video_window_t *w;
    int ie = hal_save_cli();
    hal_spin_lock( &allw_lock );
    queue_iterate(&allwindows, w, drv_video_window_t *, chain)
    {
        drv_video_winblt( w );
        if(w->flags & WFLAG_WIN_DECORATED)
            win_make_decorations(w);
    }
    hal_spin_unlock( &allw_lock );
    if(ie) hal_sti();
}

// TODO this is not needed anymore?
void drv_video_window_preblit( drv_video_window_t *w )
{
#if 0
	//window_basic_border( w, brdr, sizeof(brdr)/sizeof(rgba_t) );
    if( (w->generation != win_generation) )
    {
        w->generation = win_generation;
        if(w->flags & WFLAG_WIN_DECORATED)
            win_make_decorations(w);
    }
#endif
}

static void
do_window_resize( drv_video_window_t *w, int xsize, int ysize )
{
    panic("not impl");
}






void
drv_video_window_move( drv_video_window_t *w, int x, int y )
{
    int ox = w->x;
    int oy = w->x;
    video_zbuf_reset_square( w->x, w->y, w->xsize, w->ysize );
    w->x = x;
    w->y = y;

    ui_event_t e;
    //struct ui_event e;

    e.w.info = UI_EVENT_GLOBAL_REPAINT_RECT;
    e.w.rect.x = ox;
    e.w.rect.y = oy;
    e.w.rect.xsize = w->xsize;
    e.w.rect.ysize = w->ysize;

    event_q_put_global( &e );

    event_q_put_win( 0, 0, UI_EVENT_WIN_REPAINT, w );

}


void
drv_video_window_resize( drv_video_window_t *w, int xsize, int ysize )
{
    rect_t oldsize;
    rect_t newsize;
    rect_t maxsize;
    drv_video_window_get_bounds( w, &oldsize );
    do_window_resize(w, xsize, ysize);
    drv_video_window_get_bounds( w, &newsize );

    rect_add( &maxsize, &oldsize, &newsize );

    video_zbuf_reset_square( maxsize.x, maxsize.y, maxsize.xsize, maxsize.ysize );

    {
    ui_event_t e;
    //struct ui_event e;
    e.w.info = UI_EVENT_GLOBAL_REPAINT_RECT;
    e.w.rect = maxsize;

    event_q_put_global( &e );
    }
}


void
drv_video_window_get_bounds( drv_video_window_t *w, rect_t *out )
{
    assert(out);

    out->x = w->x;
    out->y = w->y;
    out->xsize = w->xsize;
    out->ysize = w->ysize;
}



