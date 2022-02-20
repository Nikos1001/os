
#ifndef VIDEO_H
#define VIDEO_H

#include "../../util.h"

struct video_framebuffer {
    uchar* fb;
    uint w, h;
    uint pitch;
};

extern struct video_framebuffer gfx;

void video_init(uint w, uint h);

void video_pushFrame(void);

#endif