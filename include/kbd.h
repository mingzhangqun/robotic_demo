#ifndef __KBD_H__
#define __KBD_H__

#include "common.h"


#define KBD_PATH    "/dev/input/by-path/platform-xhci-hcd.3.auto-usb-0:1.1:1.0-event-kbd"


int kbd_init(void);
int kbd_deinit(void);
int kbd_read(struct input_event *ev);


#endif // __KBD_H__
