#include "pwm.h"


int motor_init(void)
{
    run_cmd("echo 334 > /sys/class/gpio/unexport", NULL, 0);
    run_cmd("echo 314 > /sys/class/gpio/unexport", NULL, 0);
    run_cmd("echo 320 > /sys/class/gpio/unexport", NULL, 0);

    run_cmd("echo 334 > /sys/class/gpio/export", NULL, 0);
    run_cmd("echo 314 > /sys/class/gpio/export", NULL, 0);
    run_cmd("echo 320 > /sys/class/gpio/export", NULL, 0);

    run_cmd("echo out > /sys/class/gpio/gpio334/direction", NULL, 0);
    run_cmd("echo out > /sys/class/gpio/gpio314/direction", NULL, 0);
    run_cmd("echo out > /sys/class/gpio/gpio320/direction", NULL, 0);

    return 0;
}

int motor_deinit(void)
{
    motor_state(0);

    run_cmd("echo 334 > /sys/class/gpio/unexport", NULL, 0);
    run_cmd("echo 314 > /sys/class/gpio/unexport", NULL, 0);
    run_cmd("echo 320 > /sys/class/gpio/unexport", NULL, 0);

    return 0;
}

int motor_state(int state)
{
    if (0 == state) { // stop
        run_cmd("echo 0 > /sys/class/gpio/gpio334/value", NULL, 0);
        run_cmd("echo 0 > /sys/class/gpio/gpio314/value", NULL, 0);
        run_cmd("echo 0 > /sys/class/gpio/gpio320/value", NULL, 0);
    }
    else if (1 == state) { // foreward
	run_cmd("echo 0 > /sys/class/gpio/gpio334/value", NULL, 0);
        run_cmd("echo 1 > /sys/class/gpio/gpio314/value", NULL, 0);
        run_cmd("echo 1 > /sys/class/gpio/gpio320/value", NULL, 0);
    }
    else if (2 == state) { // backward
	run_cmd("echo 1 > /sys/class/gpio/gpio334/value", NULL, 0);
        run_cmd("echo 0 > /sys/class/gpio/gpio314/value", NULL, 0);
        run_cmd("echo 1 > /sys/class/gpio/gpio320/value", NULL, 0);
    }

    return 0;
}
