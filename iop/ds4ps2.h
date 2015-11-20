#ifndef DS4PS2_H
#define DS4PS2_H

#define DS4PS2_BIND_RPC_ID 0xD2472200

#define DS4_VID 0x054C
#define DS4_PID 0x05C4

#define DS4PS2_MAX_SLOTS 2

enum DS4PS2_SLOTS {
	DS4PS2_SLOT_1,
	DS4PS2_SLOT_2
};

enum ds4ps2_commands {
	DS4PS2_SET_LED_COLOR,     //(slot, r,g,b)
	DS4PS2_SET_LED_DURATION,  //(slot, on, off)
	DS4PS2_SET_RUMBLE,        //(slot, fast, slow)
	DS4PS2_SEND_LEDSRUMBLE,   //(slot)
	DS4PS2_GET_INPUT,         //(slot, struct)
	DS4PS2_SLOT_CONNECTED     //(slot)
};

struct ds4_report {
	u8 report_ID;
	u8 leftX;
	u8 leftY;
	u8 rightX;
	u8 rightY;

	u8 dpad     : 4;
	u8 square   : 1;
	u8 cross    : 1;
	u8 circle   : 1;
	u8 triangle : 1;

	u8 L1      : 1;
	u8 R1      : 1;
	u8 L2      : 1;
	u8 R2      : 1;
	u8 share   : 1;
	u8 options : 1;
	u8 L3      : 1;
	u8 R3      : 1;

	u8 PS   : 1;
	u8 tpad : 1;
	u8 cnt1 : 6;

	u8 Ltrigger;
	u8 Rtrigger;

	u8 cnt2;
	u8 cnt3;

	u8 battery;

	s16 accelX;
	s16 accelY;
	s16 accelZ;

	union {
		s16 roll;
		s16 gyroZ;
	};
	union {
		s16 yaw;
		s16 gyroY;
	};
	union {
		s16 pitch;
		s16 gyroX;
	};

	u8 unk1[5];

	u8 battery_level : 4;
	u8 usb_plugged   : 1;
	u8 headphones    : 1;
	u8 microphone    : 1;
	u8 padding       : 1;

	u8 unk2[2];
	u8 trackpadpackets;
	u8 packetcnt;

	u32 finger1ID     : 7;
	u32 finger1active : 1;
	u32 finger1X      : 12;
	u32 finger1Y      : 12;

	u32 finger2ID     : 7;
	u32 finger2active : 1;
	u32 finger2X      : 12;
	u32 finger2Y      : 12;

} __attribute__((packed, aligned(32)));

#define DS4PS2_REPORT_LEN (sizeof(struct ds4_report))

#endif
