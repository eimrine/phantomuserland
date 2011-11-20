#include "video_drv_basic_vga.h"

char basic_VGA_Pal[768] = {
 0,  0,  0,  0,  0, 42,  0, 42,  0,  0, 42, 42, 42,  0,  0, 42,  0, 42,
42, 21,  0, 42, 42, 42, 21, 21, 21, 21, 21, 63, 21, 63, 21, 21, 63, 63,
63, 21, 21, 63, 21, 63, 63, 63, 21, 63, 63, 63,  0,  0,  0,  5,  5,  5,
 8,  8,  8, 11, 11, 11, 14, 14, 14, 17, 17, 17, 20, 20, 20, 24, 24, 24,
28, 28, 28, 32, 32, 32, 36, 36, 36, 40, 40, 40, 45, 45, 45, 50, 50, 50,
56, 56, 56, 63, 63, 63,  0,  0, 63, 16,  0, 63, 31,  0, 63, 47,  0, 63,
63,  0, 63, 63,  0, 47, 63,  0, 31, 63,  0, 16, 63,  0,  0, 63, 16,  0,
63, 31,  0, 63, 47,  0, 63, 63,  0, 47, 63,  0, 31, 63,  0, 16, 63,  0,
 0, 63,  0,  0, 63, 16,  0, 63, 31,  0, 63, 47,  0, 63, 63,  0, 47, 63,
 0, 31, 63,  0, 16, 63, 31, 31, 63, 39, 31, 63, 47, 31, 63, 55, 31, 63,
63, 31, 63, 63, 31, 55, 63, 31, 47, 63, 31, 39, 63, 31, 31, 63, 39, 31,
63, 47, 31, 63, 55, 31, 63, 63, 31, 55, 63, 31, 47, 63, 31, 39, 63, 31,
31, 63, 31, 31, 63, 39, 31, 63, 47, 31, 63, 55, 31, 63, 63, 31, 55, 63,
31, 47, 63, 31, 39, 63, 45, 45, 63, 49, 45, 63, 54, 45, 63, 58, 45, 63,
63, 45, 63, 63, 45, 58, 63, 45, 54, 63, 45, 49, 63, 45, 45, 63, 49, 45,
63, 54, 45, 63, 58, 45, 63, 63, 45, 58, 63, 45, 54, 63, 45, 49, 63, 45,
45, 63, 45, 45, 63, 49, 45, 63, 54, 45, 63, 58, 45, 63, 63, 45, 58, 63,
45, 54, 63, 45, 49, 63,  0,  0, 28,  7,  0, 28, 14,  0, 28, 21,  0, 28,
28,  0, 28, 28,  0, 21, 28,  0, 14, 28,  0,  7, 28,  0,  0, 28,  7,  0,
28, 14,  0, 28, 21,  0, 28, 28,  0, 21, 28,  0, 14, 28,  0,  7, 28,  0,
 0, 28,  0,  0, 28,  7,  0, 28, 14,  0, 28, 21,  0, 28, 28,  0, 21, 28,
 0, 14, 28,  0,  7, 28, 14, 14, 28, 17, 14, 28, 21, 14, 28, 24, 14, 28,
28, 14, 28, 28, 14, 24, 28, 14, 21, 28, 14, 17, 28, 14, 14, 28, 17, 14,
28, 21, 14, 28, 24, 14, 28, 28, 14, 24, 28, 14, 21, 28, 14, 17, 28, 14,
14, 28, 14, 14, 28, 17, 14, 28, 21, 14, 28, 24, 14, 28, 28, 14, 24, 28,
14, 21, 28, 14, 17, 28, 20, 20, 28, 22, 20, 28, 24, 20, 28, 26, 20, 28,
28, 20, 28, 28, 20, 26, 28, 20, 24, 28, 20, 22, 28, 20, 20, 28, 22, 20,
28, 24, 20, 28, 26, 20, 28, 28, 20, 26, 28, 20, 24, 28, 20, 22, 28, 20,
20, 28, 20, 20, 28, 22, 20, 28, 24, 20, 28, 26, 20, 28, 28, 20, 26, 28,
20, 24, 28, 20, 22, 28,  0,  0, 16,  4,  0, 16,  8,  0, 16, 12,  0, 16,
16,  0, 16, 16,  0, 12, 16,  0,  8, 16,  0,  4, 16,  0,  0, 16,  4,  0,
16,  8,  0, 16, 12,  0, 16, 16,  0, 12, 16,  0,  8, 16,  0,  4, 16,  0,
 0, 16,  0,  0, 16,  4,  0, 16,  8,  0, 16, 12,  0, 16, 16,  0, 12, 16,
 0,  8, 16,  0,  4, 16,  8,  8, 16, 10,  8, 16, 12,  8, 16, 14,  8, 16,
16,  8, 16, 16,  8, 14, 16,  8, 12, 16,  8, 10, 16,  8,  8, 16, 10,  8,
16, 12,  8, 16, 14,  8, 16, 16,  8, 14, 16,  8, 12, 16,  8, 10, 16,  8,
 8, 16,  8,  8, 16, 10,  8, 16, 12,  8, 16, 14,  8, 16, 16,  8, 14, 16,
 8, 12, 16,  8, 10, 16, 11, 11, 16, 12, 11, 16, 13, 11, 16, 15, 11, 16,
16, 11, 16, 16, 11, 15, 16, 11, 13, 16, 11, 12, 16, 11, 11, 16, 12, 11,
16, 13, 11, 16, 15, 11, 16, 16, 11, 15, 16, 11, 13, 16, 11, 12, 16, 11,
11, 16, 11, 11, 16, 12, 11, 16, 13, 11, 16, 15, 11, 16, 16, 11, 15, 16,
11, 13, 16, 11, 12, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 0,  0,  0,  0,  0,  0,  0,  0,  0, 63, 63, 63
};



//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//=- VGA register variables for the various videomodes                       -=
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Same as Mode 01H
unsigned char basic_VGA_mode00h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x67,      0x00,          0x03,0x08,0x03,0x00,0x02,
// CRTC regs
0x2D,0x27,0x28,0x90,0x2B,0xA0,0xBF,0x1F,0x00,0x4F,0x06,0x07,0x00,0x00,0x00,
0x31,0x9C,0x8E,0x8F,0x14,0x1F,0x96,0xB9,0xA3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x10,0x0E,0x00,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x10,0x11,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x0C,0x00,0x0F,0x08,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


// Same as Mode 02H
unsigned char basic_VGA_mode03h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x67,      0x00,          0x03,0x00,0x03,0x00,0x02,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x55,0x81,0xBF,0x1F,0x00,0x4F,0x0E,0x0F,0x00,0x00,0x00,
0x00,0x9C,0x0E,0x8F,0x28,0x01,0x96,0xB9,0xA3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x10,0x0E,0x00,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x10,0x11,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x0C,0x00,0x0F,0x08,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Same as Mode 05,
unsigned char basic_VGA_mode04h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x63,      0x00,          0x03,0x09,0x03,0x00,0x02,
// CRTC regs
0x2D,0x27,0x28,0x90,0x2B,0x80,0x0BF,0x1F,0x00,0x0C1,0x00,0x00,0x00,0x00,0x00,
0x31,0x9C,0x8E,0x8F,0x14,0x00,0x96,0xB9,0x0A2,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x30,0x0F,0x00,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x3B,0x3D,0x3F,0x02,0x04,0x06,0x07,0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
0x01,0x00,0x03,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_mode06h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x63,      0x00,          0x03,0x01,0x01,0x00,0x06,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x54,0x80,0x0BF,0x1F,0x00,0x0C1,0x00,0x00,0x00,0x00,0x00,
0x00,0x9C,0x8E,0x8F,0x28,0x00,0x96,0xB9,0x0C2,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x00,0x0D,0x00,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
0x01,0x00,0x01,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_mode07h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x066,     0x00,          0x03,0x00,0x03,0x00,0x02,
// CRTC regs
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x10,0x0A,0x00,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
0x0E,0x00,0x0F,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_mode0Dh[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x063,     0x00,          0x03,0x09,0x0F,0x00,0x06,
// CRTC regs
0x2D,0x27,0x28,0x90,0x2B,0x80,0x0BF,0x1F,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,
0x31,0x9C,0x8E,0x8F,0x14,0x00,0x96,0xB9,0xE3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x10,0x05,0x00,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x10,0x11,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x01,0x00,0x0F,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_mode0Eh[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x63,      0x00,          0x03,0x01,0x0F,0x00,0x06,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x54,0x80,0x0BF,0x1F,0x00,0xC0,0x00,0x00,0x00,0x00,0x00,
0x59,0x9C,0x8E,0x8F,0x28,0x00,0x96,0xB9,0xE3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x0F,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x08,0x11,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x01,0x00,0x0F,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_mode0Fh[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x0A2,     0x00,          0x03,0x01,0x0F,0x00,0x06,
// CRTC regs
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x08,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x08,0x00,0x00,0x00,0x18,0x00,0x00,
0x0B,0x00,0x05,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_mode10h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x0A3,     0x00,          0x03,0x01,0x0F,0x00,0x06,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x54,0x80,0x0BF,0x1F,0x00,0x40,0x00,0x00,0x00,0x00,0x00,
0x00,0x83,0x85,0x5D,0x28,0x0F,0x63,0x0BA,0xE3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x0F,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
0x01,0x00,0x0F,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_mode11h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0xE3,     0x00,          0x03,0x01,0x0F,0x00,0x06,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x54,0x80,0x0B,0x3E,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x59,
0xEA,0x8C,0x0DF,0x28,0x0F,0x0E7,0x004,0x0C3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x08,0x3F,0x3F,0x18,0x18,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,0x3F,
0x01,0x00,0x0F,0x00,0x00

};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_mode12h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0xE3,     0x00,          0x03,0x01,0x0F,0x00,0x06,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x54,0x80,0x0B,0x3E,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x59,
0xEA,0x8C,0x0DF,0x28,0x00,0x0E7,0x04,0xE3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x0F,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
0x01,0x00,0x0F,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_mode13h[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x63,      0x00,          0x03,0x01,0x0F,0x00,0x0E,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x54,0x80,0x0BF,0x1F,0x00,0x41,0x00,0x00,0x00,0x00,0x00,
0x00,0x9C,0x0E,0x8F,0x28,0x40,0x96,0xB9,0x0A3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x40,0x05,0x0F,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
0x41,0x00,0x0F,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_modeC4[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x63,      0x00,          0x03,0x01,0x0F,0x00,0x06,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x54,0x80,0x0BF,0x1F,0x00,0x41,0x00,0x00,0x00,0x00,0x00,0x00,
0x9C,0x0E,0x8F,0x28,0x00,0x96,0xB9,0xE3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x40,0x05,0x0F,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
0x41,0x00,0x0F,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_modeJ[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0xa3,      0x00,          0x03,0x01,0x03,0x00,0x02,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x55,0x81,0xBF,0x1F,0x00,0x47,0x0E,0x0F,0x00,0x00,0x00,
0x00,0x83,0x85,0x57,0x28,0x1F,0x60,0xB8,0xA3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x10,0x0E,0x00,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x10,0x11,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x0C,0x00,0x0F,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_modeK[62] = {
// MISC reg,  STATUS reg,    SEQ regs
   0x63,      0x00,          0x03,0x01,0x03,0x00,0x02,
// CRTC regs
0x5F,0x4F,0x50,0x82,0x55,0x81,0xBF,0x1F,0x00,0x47,0x0E,0x0F,0x00,0x00,0x00,
0x00,0x9C,0x8E,0x8F,0x28,0x1F,0x96,0xB9,0xA3,0xFF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x10,0x0E,0x00,0xFF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x10,0x11,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x0C,0x00,0x0F,0x00,0x00
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

unsigned char basic_VGA_modeL[62] = {
// MISC reg,  STATUS reg,    SEQ regs
0x67,0x00,0x03,0x08,0x03,0x00,0x02,
// CRTC regs
0x2D,0x27,0x28,0x90,0x2B,0x0A0,0x0BF,0x1F,0x00,0x47,0x06,0x07,0x00,0x00,0x00,
0x31,0x83,0x85,0x57,0x14,0x1F,0x60,0x0B8,0x0A3,0x0FF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x10,0x0E,0x00,0x0FF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x10,0x11,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x0C,0x00,0x0F,0x00,0x00,
};
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


unsigned char basic_VGA_modeM[62] = {
// MISC reg,  STATUS reg,    SEQ regs
0x67,0x00,0x03,0x08,0x03,0x00,0x02,
// CRTC regs
0x2D,0x27,0x28,0x90,0x2B,0x0A0,0x0BF,0x1F,0x00,0x47,0x06,0x07,0x00,0x00,0x00,0x31,
0x9C,0x8E,0x8F,0x14,0x1F,0x96,0x0B9,0x0A3,0x0FF,
// GRAPHICS regs
0x00,0x00,0x00,0x00,0x00,0x10,0x0E,0x00,0x0FF,
// ATTRIBUTE CONTROLLER regs
0x00,0x01,0x02,0x03,0x04,0x05,0x14,0x07,0x38,0x39,0x3A,0x3B,0x3C,0x3D,0x3E,0x3F,
0x0C,0x00,0x0F,0x00,0x00,
};

