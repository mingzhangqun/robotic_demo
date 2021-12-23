#include "kbd.h"


//int kdb_fd = -1;


int kbd_init(char *path)
{
    int ret = 0;
    int fd = -1;

    fd = open(path, O_RDONLY);
    
    return fd;
}


int kbd_deinit(int fd)
{
    int ret = 0;

    if (fd > 0) {
        ret = close(fd);
    }

    return ret;
}


int kbd_read(int fd, struct input_event *ev)
{
    

    if (sizeof(*ev) == read(fd, ev, sizeof(*ev))) {
        return 0;
    }

    return -1;
}