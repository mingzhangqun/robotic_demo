#include "kbd.h"


int kdb_fd = -1;


int kbd_init(void)
{
    int ret = 0;

    kdb_fd = open(KBD_PATH, O_RDONLY);
    if (kdb_fd <= 0) {
        ret = -1;
    }

    return ret;
}


int kbd_deinit(void)
{
    int ret = 0;

    if (kdb_fd > 0) {
        ret = close(kdb_fd);
    }

    return ret;
}


int kbd_read(struct input_event *ev)
{
    if (sizeof(*ev) == read(kdb_fd, ev, sizeof(*ev))) {
        return 0;
    }

    return -1;
}