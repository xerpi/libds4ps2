#ifndef LIBDS4PS2_H
#define LIBDS4PS2_H

#include <ds4ps2.h>
#include <kernel.h>

#ifdef __cplusplus
extern "C" {
#endif


int ds4ps2_init();
int ds4ps2_slot_connected(int slot);
int ds4ps2_set_led_color(int slot, u8 r, u8 g, u8 b);
int ds4ps2_set_led_duration(int slot, u8 time_on, u8 time_off);
int ds4ps2_set_rumble(int slot, u8 fast, u8 slow);
int ds4ps2_send_ledsrumble(int slot);
int ds4ps2_get_input(int slot, struct ds4_report *report);


#ifdef __cplusplus
}
#endif

#endif
