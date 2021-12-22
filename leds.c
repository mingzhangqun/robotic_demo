#include "common.h"


#define MAX_LEDS       3


int leds_init(void)
{
    run_cmd("echo none > /sys/class/leds/beaglebone:green:usr0/trigger", NULL, 0);
    run_cmd("echo none > /sys/class/leds/beaglebone:green:usr1/trigger", NULL, 0);
    run_cmd("echo none > /sys/class/leds/beaglebone:green:usr2/trigger", NULL, 0);

    return 0;
}

int leds_deinit(void)
{
    run_cmd("echo heartbeat > /sys/class/leds/beaglebone:green:usr0/trigger", NULL, 0);
    run_cmd("echo mmc0 > /sys/class/leds/beaglebone:green:usr1/trigger", NULL, 0);
    run_cmd("echo cpu > /sys/class/leds/beaglebone:green:usr2/trigger", NULL, 0);

    return 0;
}

int leds_ctrl(uint32_t leds)
{
    int i = 0;
    char buf[64];

    for (i=0; i<MAX_LEDS; i++) {
        sprintf(buf, "echo %d > /sys/class/leds/beaglebone:green:usr%d/brightness", !!(leds & (0x01<<i)), i);
        run_cmd(buf, NULL, 0);
    }

    return 0;
}
