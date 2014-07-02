#include <thbase.h>
#include <sifcmd.h>
#include <usbd.h>
#include <usbd_macro.h>
#include <string.h>
#include "ds4ps2.h"

static void  rpc_thread(void *data);
static void *rpc_server_func(int command, void *buffer, int size);

static SifRpcDataQueue_t  rpc_queue  __attribute__((aligned(64)));
static SifRpcServerData_t rpc_server __attribute__((aligned(64)));

static int _rpc_buffer[512] __attribute((aligned(64)));
static u8 data_buf[DS4PS2_MAX_SLOTS][DS4PS2_REPORT_LEN] __attribute((aligned(64)));

static int usb_probe(int devId);
static int usb_connect(int devId);
static int usb_disconnect(int devId);

static UsbDriver driver = { NULL, NULL, "ds4ps2", usb_probe, usb_connect, usb_disconnect };

static void request_data(int result, int count, void *arg);
static void config_set(int result, int count, void *arg);
static int send_ledsrumble(int slot);
static void set_led_color(int slot, unsigned char r, unsigned char g,  unsigned char b);

static struct {
    int devID;
    int connected;
    int inputEndp;
    int outputEndp;
    int controlEndp;

    struct {
        unsigned char r, g, b;
        unsigned char time_on, time_off;
    } led;
    struct {
        unsigned char fast, slow;
    } rumble;
} ds4_list[DS4PS2_MAX_SLOTS];

int _start()
{
    iop_thread_t th = {
        .attr      = TH_C,
        .thread    = rpc_thread,
        .priority  = 40,
        .stacksize = 0x800,
        .option    = 0
    };
    
    UsbRegisterDriver(&driver);
    
    int thid = CreateThread(&th);
    if (thid > 0) {
        StartThread(thid, NULL);
        return 0;   
    }
    return 1;
}


void rpc_thread(void *data)
{
    int thid = GetThreadId();
    
    SifInitRpc(0);
    SifSetRpcQueue(&rpc_queue, thid);
    SifRegisterRpc(&rpc_server, DS4PS2_BIND_RPC_ID, rpc_server_func,
        _rpc_buffer, NULL, NULL, &rpc_queue);
    SifRpcLoop(&rpc_queue);
}

int usb_probe(int devId)
{
    UsbDeviceDescriptor *dev = NULL;
    dev = UsbGetDeviceStaticDescriptor(devId, NULL, USB_DT_DEVICE);
    if (!dev)
        return 0;
    
    if (dev->idVendor == DS4_VID && dev->idProduct == DS4_PID) {
        //Check if there's an available slot
        if (ds4_list[0].connected && ds4_list[1].connected) return 0;
        return 1;
    }
    
    return 0;
}

int usb_connect(int devId)
{
    UsbDeviceDescriptor    *dev;
    UsbConfigDescriptor    *conf;
    UsbInterfaceDescriptor *intf;
    UsbEndpointDescriptor  *endp;
    
    int configEndp;

    dev = UsbGetDeviceStaticDescriptor(devId, NULL, USB_DT_DEVICE);
    conf = UsbGetDeviceStaticDescriptor(devId, dev, USB_DT_CONFIG);
    configEndp = UsbOpenEndpoint(devId, NULL);
    
    int slot = 0;
    if (ds4_list[0].connected) slot = 1;
    ds4_list[slot].connected = 1;
    ds4_list[slot].devID = devId;
    
    intf = (UsbInterfaceDescriptor *) ((char *) conf + conf->bLength);
    endp = (UsbEndpointDescriptor *) ((char *) intf + intf->bLength); /* HID endpoint */
    ds4_list[slot].controlEndp = UsbOpenEndpoint(devId, endp);
    endp = (UsbEndpointDescriptor *) ((char *) endp + endp->bLength); /* Interrupt Input endpoint */
    ds4_list[slot].inputEndp = UsbOpenEndpoint(devId, endp);
    endp = (UsbEndpointDescriptor *) ((char *) endp + endp->bLength); /* Interrupt Output endpoint */
    ds4_list[slot].outputEndp = UsbOpenEndpoint(devId, endp);

    UsbSetDevicePrivateData(devId, NULL);
    UsbSetDeviceConfiguration(configEndp, conf->bConfigurationValue, config_set, (void*)slot);
    return 0;
}

int usb_disconnect(int devId)
{
    if (devId == ds4_list[0].devID) ds4_list[0].connected = 0;
    else ds4_list[1].connected = 0;
    return 1;
}

static const unsigned char slot_rgb_colors[DS4PS2_MAX_SLOTS][3] = {
    {0xFF, 0x00, 0X00},
    {0x00, 0xFF, 0X00}
};

static void config_set(int result, int count, void *arg)
{
    int slot = (int)arg;
    
    //Set LED color
    unsigned char *c = (unsigned char *)&slot_rgb_colors[slot];
    set_led_color(slot, c[0], c[1], c[2]);
    send_ledsrumble(slot);
    //Start reading!
    request_data(0, 0, (void *)slot);
}

#define INTERFACE_GET (USB_DIR_IN|USB_TYPE_CLASS|USB_RECIP_INTERFACE)
#define INTERFACE_SET (USB_DIR_OUT|USB_TYPE_CLASS|USB_RECIP_INTERFACE)
#define USB_REPTYPE_INPUT       0x01
#define USB_REPTYPE_OUTPUT      0x02
#define USB_REPTYPE_FEATURE     0x03


static void correct_data(struct ds4_report *data)
{
    data->finger1active = !data->finger1active;
    data->finger2active = !data->finger2active;
}

static void request_data_cb(int result, int count, void *arg)
{
    int slot = (int)arg;
    correct_data((struct ds4_report *)data_buf[slot]);
    request_data(0, 0, (void *)slot);
}

static void request_data(int result, int count, void *arg)
{
    int slot = (int)arg;
    UsbInterruptTransfer(ds4_list[slot].inputEndp,
        data_buf[slot],
        DS4PS2_REPORT_LEN,
        request_data_cb,
        arg);
}


u8 __attribute__((aligned(64))) ledsrumble_buf[] = {
        0x05, //Report ID
        0xFF, 0x00, 0x00,
        0x00, //Fast motor
        0x00, //Slow motor
        0x00, 0x00, 0xFF, // RGB
        0x00, //LED on duration
        0x00  //LED off duration
};

static int send_ledsrumble(int slot)
{
    ledsrumble_buf[4]  = ds4_list[slot].rumble.fast;
    ledsrumble_buf[5]  = ds4_list[slot].rumble.slow;
    ledsrumble_buf[6]  = ds4_list[slot].led.r;
    ledsrumble_buf[7]  = ds4_list[slot].led.g;
    ledsrumble_buf[8]  = ds4_list[slot].led.b;
    ledsrumble_buf[9]  = ds4_list[slot].led.time_on;
    ledsrumble_buf[10] = ds4_list[slot].led.time_off;

    return UsbInterruptTransfer(ds4_list[slot].outputEndp,
        ledsrumble_buf,
        sizeof(ledsrumble_buf),
        NULL, NULL);
}

static void set_led_color(int slot, unsigned char r, unsigned char g, unsigned char b)
{
    ds4_list[slot].led.r = r;
    ds4_list[slot].led.g = g;
    ds4_list[slot].led.b = b;
}

static void set_led_duration(int slot, unsigned char time_on, unsigned char time_off)
{
    ds4_list[slot].led.time_on  = time_on;
    ds4_list[slot].led.time_off = time_off;
}

static void set_rumble(int slot, unsigned char fast, unsigned char slow)
{
    ds4_list[slot].rumble.fast = fast;
    ds4_list[slot].rumble.slow = slow;
}


void *rpc_server_func(int command, void *buffer, int size)
{
    u8 *b8 = (u8*)buffer;
    int slot = b8[0];
    
    switch (command) {
    case DS4PS2_SET_LED_COLOR:
        set_led_color(slot, b8[1], b8[2], b8[3]);
        break;
    case DS4PS2_SET_LED_DURATION:
        set_led_duration(slot, b8[1], b8[2]);
        break;
    case DS4PS2_SET_RUMBLE:
        set_rumble(slot, b8[1], b8[2]);
        break;
    case DS4PS2_SEND_LEDSRUMBLE:
        send_ledsrumble(slot);
        break;
    case DS4PS2_GET_INPUT:
        memcpy(buffer, data_buf[slot], size);
        break;
    case DS4PS2_SLOT_CONNECTED:
        b8[0] = ds4_list[slot].connected;
        break;
    }
    
    return buffer;
}

