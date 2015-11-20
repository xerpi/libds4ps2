#include <dmaKit.h>
#include <math.h>
#include "graphics.h"

#define DEG2RAD(x) ((x)*0.01745329251)

static GSGLOBAL *gsGlobal = NULL;
static GSFONTM *gsFontM = NULL;

void video_init()
{
	gsGlobal = gsKit_init_global();
	gsFontM = gsKit_init_fontm();

	dmaKit_init(D_CTRL_RELE_OFF,D_CTRL_MFD_OFF, D_CTRL_STS_UNSPEC,
	D_CTRL_STD_OFF, D_CTRL_RCYC_8, 1 << DMA_CHANNEL_GIF);

	// Initialize the DMAC
	dmaKit_chan_init(DMA_CHANNEL_GIF);
	dmaKit_chan_init(DMA_CHANNEL_FROMSPR);
	dmaKit_chan_init(DMA_CHANNEL_TOSPR);

	gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
	gsKit_init_screen(gsGlobal);
	gsKit_fontm_upload(gsGlobal, gsFontM);
	gsFontM->Spacing = 0.75f;
	gsKit_mode_switch(gsGlobal, GS_ONESHOT);
}

void clear_screen()
{
	gsKit_clear(gsGlobal, WHITE);
}

void flip_screen()
{
	gsKit_sync_flip(gsGlobal);
	gsKit_queue_exec(gsGlobal);
}

int screen_w()
{
	return gsGlobal->Width;
}

int screen_h()
{
	return gsGlobal->Height;
}

void font_print(float x, float y, const char *text)
{
	gsKit_fontm_print_scaled(gsGlobal, gsFontM, x, y, 3, 0.5f,
		GS_SETREG_RGBA(0x2F,0x20,0x20,0xFF), text);
}

void draw_circle(float x, float y, float radius, u32 color, u8 filled)
{
	float vertices[37*2];
	int i;

	for (i = 0; i < 36; i++) {
		float rad = DEG2RAD(i*10);
		vertices[i*2+0] = cos(rad) * radius + x;
		vertices[i*2+1] = sin(rad) * radius + y;
	}

	if (!filled) {
		vertices[72] = radius + x;
		vertices[73] = y;
	}

	if (filled)
		gsKit_prim_triangle_fan(gsGlobal, vertices, 36, 3, color);
	else
		gsKit_prim_line_strip(gsGlobal, vertices, 37, 3, color);
}

