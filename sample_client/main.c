#include <gsKit.h>
#include <dmaKit.h>
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <gsToolkit.h>
#include <libds4ps2.h>

void video_init();
void clear_screen();
void flip_screen();
GSGLOBAL *gsGlobal;
GSFONTM *gsFontM;
u64 White, Black, FontColor, Red, Blue, CircleColor;
#define font_print(x, y, text) \
    gsKit_fontm_print_scaled(gsGlobal, gsFontM, x, y, 3, 0.5f, FontColor, text) 

#define DEG2RAD(x) ((x)*0.01745329251)
u8 r = 0xFF, g=0x00, b=0x00;
void draw_circle(GSGLOBAL *gsGlobal, float x, float y, float radius, u64 color, u8 filled);
int print_data(int y, struct ds4_report *data);
void correct_data(struct ds4_report *data);
void random_leds();


int main(void)
{
    video_init();
    
    int ret = SifLoadModule("mass:/ds4ps2.irx", 0, NULL);
    if (ret < 0) {
        char *txt = "Could not find 'mass:/ds4ps2.irx'";
        while (1) {
            clear_screen();
            font_print(5, 10, txt);
            flip_screen();
        }
    }
    
    struct ds4_report ds4_1, ds4_2;
    memset(&ds4_1, 0x0, sizeof(struct ds4_report));
    memset(&ds4_2, 0x0, sizeof(struct ds4_report)); 
    ds4ps2_init();
    
    float pos_x = gsGlobal->Width/2, pos_y = gsGlobal->Height/2;
    char text_connected[64];
    
    while (1) {
        clear_screen();

        ds4ps2_get_input(DS4PS2_SLOT_1, &ds4_1);
        ds4ps2_get_input(DS4PS2_SLOT_2, &ds4_2);
        
        if (ds4_1.L1) {pos_x = gsGlobal->Width/2, pos_y = gsGlobal->Height/2;}
        if (ds4_1.R1) {random_leds();}
        //1920x940
        if (ds4_1.finger1active) {
            draw_circle(gsGlobal, (gsGlobal->Width/1920.0f)*ds4_1.finger1X, (gsGlobal->Height/940.0f)*ds4_1.finger1Y, 14, Red, 1);
        }
        if (ds4_1.finger2active) {
            draw_circle(gsGlobal, (gsGlobal->Width/1920.0f)*ds4_1.finger2X, (gsGlobal->Height/940.0f)*ds4_1.finger2Y, 14, Blue, 1);
        }

        #define THRESHOLD 25.0f
        if (fabs(ds4_1.accelX) > THRESHOLD)
            pos_y -= ds4_1.accelX/55.0f;
        if (fabs(ds4_1.accelY) > THRESHOLD)
            pos_x -= ds4_1.accelY/55.0f;
        
        sprintf(text_connected, "connected: SLOT_1 %i   SLOT_2 %i", ds4ps2_slot_connected(DS4PS2_SLOT_1),
            ds4ps2_slot_connected(DS4PS2_SLOT_2));
        font_print(5, 10, text_connected);
     
        int y = print_data(10, &ds4_1);
        print_data(y+10, &ds4_2);
        
        draw_circle(gsGlobal, pos_x, pos_y, 19, CircleColor, 0);
        draw_circle(gsGlobal, pos_x, pos_y, 18, CircleColor, 0);
        draw_circle(gsGlobal, pos_x, pos_y, 17, CircleColor, 0);
        draw_circle(gsGlobal, pos_x, pos_y, 16, CircleColor, 0);

        flip_screen();
    }

    return 0;
}

void random_leds()
{
    r = rand()%0xFF;
    g = rand()%0xFF;
    b = rand()%0xFF;
    CircleColor = GS_SETREG_RGBAQ(r, g, b, 0x00, 0x00);
    ds4ps2_set_led_color(DS4PS2_SLOT_1, r, g, b);
    ds4ps2_send_ledsrumble(DS4PS2_SLOT_1);
}

int print_data(int y, struct ds4_report *data)
{ 
    char text[256];
    int x = 5;
    sprintf(text, "PS: %1i   OPTIONS: %1i  SHARE: %1i   /\\: %1i   []: %1i   O: %1i   X: %1i\n", \
            data->PS, data->options, data->share, data->triangle, \
            data->square, data->circle, data->cross);
    font_print(x, y+=30, text);
    
    sprintf(text, "TPAD: %1i   L3: %1i   R3: %1i\n", \
            data->tpad, data->L3, data->R3);
    font_print(x, y+=30, text);

    sprintf(text, "L1: %1i   L2: %1i   R1: %1i   R2: %1i   DPAD: %1i\n", \
            data->L1, data->L2, data->R1, data->R2, \
            data->dpad);
    font_print(x, y+=30, text);
            
    sprintf(text, "LX: %2X   LY: %2X   RX: %2X   RY: %2X  battery: %1X\n", \
            data->leftX, data->leftY, data->rightX, data->rightY, data->battery);
    font_print(x, y+=30, text);
    
    sprintf(text, "headphones: %1X   mic: %1X   usb: %1X  batt: %2X\n", \
            data->headphones, data->microphone, data->usb_plugged, data->battery_level);
    font_print(x, y+=30, text);

    sprintf(text, "aX: %5i  aY: %5i  aZ: %5i  gX: %5i  gY: %5i  gZ: %5i\n", \
            data->accelX, data->accelY, data->accelZ, data->gyroX, data->gyroY, data->gyroZ);
    font_print(x, y+=30, text);

    sprintf(text, "Ltrig: %2X   Rtrig: %2X  tpadpck: %4i  pcktcnt: %4i\n", \
            data->Ltrigger, data->Rtrigger, data->trackpadpackets, data->packetcnt);
    font_print(x, y+=30, text);
            
    sprintf(text, "f1active: %2X   f1ID: %2X  f1X: %4i  f1Y: %4i\n", \
            data->finger1active, data->finger1ID, data->finger1X, data->finger1Y);
    font_print(x, y+=30, text);
    
    sprintf(text, "f2active: %2X   f2ID: %2X  f2X: %4i  f2Y: %4i\n", \
            data->finger2active, data->finger2ID, data->finger2X, data->finger2Y);
    font_print(x, y+=30, text);
    
    return y;
}


void draw_circle(GSGLOBAL *gsGlobal, float x, float y, float radius, u64 color, u8 filled)
{    
    float v[37*2];
    int a;
    float ra;
    for (a = 0; a < 36; a++) {
        ra = DEG2RAD(a*10);
        v[a*2] = cos(ra) * radius + x;
        v[a*2+1] = sin(ra) * radius + y;
    }
    if (!filled) {
        v[72] = radius + x;
        v[73] = y;
    }
    
    if (filled) gsKit_prim_triangle_fan(gsGlobal, v, 36, 3, color);
    else        gsKit_prim_line_strip(gsGlobal, v, 37, 3, color);
}

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

    Black = GS_SETREG_RGBAQ(0x00,0x00,0x00,0xFF,0x00);
    White = GS_SETREG_RGBAQ(0xFF,0xFF,0xFF,0x00,0x00);
    Red = CircleColor = GS_SETREG_RGBAQ(0xFF,0x00,0x00,0x00,0x00);
    Blue = GS_SETREG_RGBAQ(0x00,0x00,0xFF,0x00,0x00);
    FontColor = GS_SETREG_RGBAQ(0x2F,0x20,0x20,0xFF,0x00);

    gsGlobal->PrimAlphaEnable = GS_SETTING_ON;
    gsKit_init_screen(gsGlobal);
    gsKit_fontm_upload(gsGlobal, gsFontM);
    gsFontM->Spacing = 0.75f;
    gsKit_mode_switch(gsGlobal, GS_ONESHOT);
}

void clear_screen()
{
    gsKit_clear(gsGlobal, White);
}

void flip_screen()
{
    gsKit_sync_flip(gsGlobal);
    gsKit_queue_exec(gsGlobal);
}
