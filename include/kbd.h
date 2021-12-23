#ifndef __KBD_H__
#define __KBD_H__

#include "common.h"


#define KBD_REMOTER_PATH    "/dev/input/by-id/usb-4555_1031-event-kbd"
#define KBD_KEY_PATH        "/dev/input/by-path/platform-gpio-keys-event"


int kbd_init(char *path);
int kbd_deinit(int fd);
int kbd_read(int fd, struct input_event *ev);


#endif // __KBD_H__
