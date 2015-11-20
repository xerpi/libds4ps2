#include <kernel.h>
#include <sifrpc.h>
#include "libds4ps2.h"

static SifRpcClientData_t ds4ps2if __attribute__((aligned(64)));
static u8 ds4ps2_buffer[5] __attribute__((aligned(64)));
static int ds4ps2_initialized = 0;

int ds4ps2_init()
{
	if (ds4ps2_initialized)
		return 0;

	ds4ps2if.server = NULL;

	do {
		if (SifBindRpc(&ds4ps2if, DS4PS2_BIND_RPC_ID, 0) < 0) {
			return -1;
		}
		nopdelay();
	} while (!ds4ps2if.server);

	ds4ps2_initialized = 1;
	return 1;
}

int ds4ps2_slot_connected(int slot)
{
	unsigned char connected = slot;
	SifCallRpc(&ds4ps2if, DS4PS2_SLOT_CONNECTED, 0,
		&connected, 1, &connected, 1, NULL, NULL);
	return connected;
}

int ds4ps2_set_led_color(int slot, u8 r, u8 g, u8 b)
{
	ds4ps2_buffer[0] = slot;
	ds4ps2_buffer[1] = r;
	ds4ps2_buffer[2] = g;
	ds4ps2_buffer[3] = b;
	return SifCallRpc(&ds4ps2if, DS4PS2_SET_LED_COLOR, 0,
		ds4ps2_buffer, 4, NULL, 0, NULL, NULL);
}

int ds4ps2_set_led_duration(int slot, u8 time_on, u8 time_off)
{
	ds4ps2_buffer[0] = slot;
	ds4ps2_buffer[1] = time_on;
	ds4ps2_buffer[2] = time_off;
	return SifCallRpc(&ds4ps2if, DS4PS2_SET_LED_DURATION, 0,
		ds4ps2_buffer, 3, NULL, 0, NULL, NULL);
}

int ds4ps2_set_rumble(int slot, u8 fast, u8 slow)
{
	ds4ps2_buffer[0] = slot;
	ds4ps2_buffer[1] = fast;
	ds4ps2_buffer[2] = slow;
	return SifCallRpc(&ds4ps2if, DS4PS2_SET_RUMBLE, 0,
		ds4ps2_buffer, 3, NULL, 0, NULL, NULL);
}

int ds4ps2_send_ledsrumble(int slot)
{
	ds4ps2_buffer[0] = slot;
	return SifCallRpc(&ds4ps2if, DS4PS2_SEND_LEDSRUMBLE, 0,
		ds4ps2_buffer, 1, NULL, 0, NULL, NULL);
}

int ds4ps2_get_input(int slot, struct ds4_report *report)
{
	((u8 *)report)[0] = slot;
	return SifCallRpc(&ds4ps2if, DS4PS2_GET_INPUT, 0,
		(void *)report, DS4PS2_REPORT_LEN, (void *)report,
		DS4PS2_REPORT_LEN, NULL, NULL);
}
