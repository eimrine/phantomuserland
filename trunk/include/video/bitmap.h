#ifndef BITMAP_H
#define BITMAP_H

#include <video/color.h>

typedef struct drv_video_bitmap
{
    int         	xsize;
    int 		ysize;
    rgba_t              pixel[];
} drv_video_bitmap_t;

extern drv_video_bitmap_t 		close_bmp; // Window close button
extern drv_video_bitmap_t 		pin_bmp; // Window pin button

drv_video_bitmap_t *      drv_video_get_default_mouse_bmp(void);


#endif //BITMAP_H