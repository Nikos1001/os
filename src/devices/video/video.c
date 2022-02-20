
#include "video.h"
#include "../mbox/mbox.h"
#include "../uart/uart.h"

struct video_framebuffer gfx;

void video_init(uint w, uint h) {
    mbox[0] = (39 + 256) * 4;
    mbox[1] = MBOX_REQUEST;

    mbox[2] = 0x48003; // physical w/h
    mbox[3] = 8;
    mbox[4] = 8;
    mbox[5] = w;
    mbox[6] = h;

    mbox[7] = 0x48004; // virtual w/h
    mbox[8] = 8;
    mbox[9] = 8;
    mbox[10] = w;
    mbox[11] = h;

    mbox[12] = 0x48009; // virtual offset
    mbox[13] = 8;
    mbox[14] = 8;
    mbox[15] = 0;
    mbox[16] = 0;

    mbox[17] = 0x48005; // depth
    mbox[18] = 4;
    mbox[19] = 4;
    mbox[20] = 8;

    mbox[21] = 0x48006; // pixel order
    mbox[22] = 4;
    mbox[23] = 4;
    mbox[24] = 1;

    mbox[25] = 0x40001; // get framebuffer
    mbox[26] = 8;
    mbox[27] = 8;
    mbox[28] = 4096;
    mbox[29] = 0;

    mbox[30] = 0x40008; // get pitch
    mbox[31] = 4;
    mbox[32] = 4;
    mbox[33] = 0;

    mbox[34] = 0x4800b; // set palette
    mbox[35] = 1032;
    mbox[36] = 1032;
    mbox[37] = 0;
    mbox[38] = 256;
    for(uint i = 0; i < 256; i++) {
        uchar r = (i & 0xe0) >> 5;
        r = 255 * (float)r / (float)7;
        uchar g = (i & 0x1c) >> 2;
        g = 255 * (float)g / (float)7;
        uchar b = (i & 0x03) >> 0;
        b = 255 * (float)b / (float)3;
        mbox[39 + i] = (r << 0) | (g << 8) | (b << 16);
    }
    mbox[39 + 256] = MBOX_TAG_LAST;

    mbox_call(MBOX_CH_PROP);
    if(mbox[28] != 0) {
        uchar* data = (uchar*)mbox[28];
        gfx.fb = data;
        gfx.w = mbox[10];
        gfx.h = mbox[11];
        gfx.pitch = mbox[33];
    }
}

void video_pushFrame(void) {
    // TODO: implement double buffering
}