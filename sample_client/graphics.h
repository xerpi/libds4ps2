#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <gsKit.h>

#define WHITE GS_SETREG_RGBA(0xFF,0xFF,0xFF,0x00)
#define BLACK GS_SETREG_RGBA(0x00,0x00,0x00,0x00)
#define RED   GS_SETREG_RGBA(0xFF,0x00,0x00,0x00)
#define GREEN GS_SETREG_RGBA(0x00,0xFF,0x00,0x00)
#define BLUE  GS_SETREG_RGBA(0x00,0x00,0xFF,0x00)

void video_init();
void clear_screen();
void flip_screen();
int screen_w();
int screen_h();

void draw_circle(float x, float y, float radius, u32 color, u8 filled);
void font_print(float x, float y, const char *text);

#endif
