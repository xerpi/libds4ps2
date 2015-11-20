#include <stdio.h>
#include <math.h>
#include <kernel.h>
#include <sifrpc.h>
#include <loadfile.h>
#include <libds4ps2.h>
#include "graphics.h"

static u8 r = 0xFF, g = 0x00, b = 0x00;
static u32 CircleColor = RED;

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

	float pos_x = screen_w()/2, pos_y = screen_h()/2;
	char text_connected[64];

	while (1) {
		clear_screen();

		ds4ps2_get_input(DS4PS2_SLOT_1, &ds4_1);
		ds4ps2_get_input(DS4PS2_SLOT_2, &ds4_2);

		if (ds4_1.L1) {pos_x = screen_w()/2, pos_y = screen_h()/2;}
		if (ds4_1.R1) {random_leds();}
		//1920x940
		if (ds4_1.finger1active) {
			draw_circle((screen_w()/1920.0f)*ds4_1.finger1X, (screen_h()/940.0f)*ds4_1.finger1Y, 14, RED, 1);
		}
		if (ds4_1.finger2active) {
			draw_circle((screen_w()/1920.0f)*ds4_1.finger2X, (screen_h()/940.0f)*ds4_1.finger2Y, 14, BLUE, 1);
		}

		#define THRESHOLD 25.0f
		if (fabs(ds4_1.accelX) > THRESHOLD)
			pos_y -= ds4_1.accelX/55.0f;
		if (fabs(ds4_1.accelY) > THRESHOLD)
			pos_x -= ds4_1.accelY/55.0f;

		sprintf(text_connected, "connected: SLOT_1 %i	SLOT_2 %i", ds4ps2_slot_connected(DS4PS2_SLOT_1),
			ds4ps2_slot_connected(DS4PS2_SLOT_2));
		font_print(5, 10, text_connected);

		int y = print_data(10, &ds4_1);
		print_data(y+10, &ds4_2);

		draw_circle(pos_x, pos_y, 19, CircleColor, 0);
		draw_circle(pos_x, pos_y, 18, CircleColor, 0);
		draw_circle(pos_x, pos_y, 17, CircleColor, 0);
		draw_circle(pos_x, pos_y, 16, CircleColor, 0);

		flip_screen();
	}

	return 0;
}

void random_leds()
{
	r = rand()%0xFF;
	g = rand()%0xFF;
	b = rand()%0xFF;
	CircleColor = GS_SETREG_RGBA(r, g, b, 0x00);
	ds4ps2_set_led_color(DS4PS2_SLOT_1, r, g, b);
	ds4ps2_send_ledsrumble(DS4PS2_SLOT_1);
}

int print_data(int y, struct ds4_report *data)
{
	char text[256];
	int x = 5;
	sprintf(text, "PS: %1i	 OPTIONS: %1i  SHARE: %1i	/\\: %1i   []: %1i	 O: %1i	  X: %1i\n", \
		data->PS, data->options, data->share, data->triangle, \
		data->square, data->circle, data->cross);
	font_print(x, y+=30, text);

	sprintf(text, "TPAD: %1i   L3: %1i	 R3: %1i\n", \
		data->tpad, data->L3, data->R3);
	font_print(x, y+=30, text);

	sprintf(text, "L1: %1i	 L2: %1i   R1: %1i	 R2: %1i   DPAD: %1i\n", \
		data->L1, data->L2, data->R1, data->R2, \
		data->dpad);
	font_print(x, y+=30, text);

	sprintf(text, "LX: %2X	 LY: %2X   RX: %2X	 RY: %2X  battery: %1X\n", \
		data->leftX, data->leftY, data->rightX, data->rightY, data->battery);
	font_print(x, y+=30, text);

	sprintf(text, "headphones: %1X	 mic: %1X	usb: %1X  batt: %2X\n", \
		data->headphones, data->microphone, data->usb_plugged, data->battery_level);
	font_print(x, y+=30, text);

	sprintf(text, "aX: %5i	aY: %5i	 aZ: %5i  gX: %5i  gY: %5i	gZ: %5i\n", \
		data->accelX, data->accelY, data->accelZ, data->gyroX, data->gyroY, data->gyroZ);
	font_print(x, y+=30, text);

	sprintf(text, "Ltrig: %2X	Rtrig: %2X	tpadpck: %4i  pcktcnt: %4i\n", \
		data->Ltrigger, data->Rtrigger, data->trackpadpackets, data->packetcnt);
	font_print(x, y+=30, text);

	sprintf(text, "f1active: %2X   f1ID: %2X  f1X: %4i	f1Y: %4i\n", \
		data->finger1active, data->finger1ID, data->finger1X, data->finger1Y);
	font_print(x, y+=30, text);

	sprintf(text, "f2active: %2X   f2ID: %2X  f2X: %4i	f2Y: %4i\n", \
		data->finger2active, data->finger2ID, data->finger2X, data->finger2Y);
	font_print(x, y+=30, text);

	return y;
}
