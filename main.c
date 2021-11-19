#include "common.h"


int main(void)
{
    pru_start(PATH_PRU0_0);
    pru_start(PATH_PRU0_1);
    pwm_init();

    uint32_t chs = 0;
    chs = PWM_CH11;
    pwm_duty(chs, 0x12);
    pwm_peroid(chs, 0x34);
    pwm_ctrl(chs, 0x01);

STOP:
    pwm_ctrl(PWM_ALL, 0x00);
    pwm_deinit();
    pru_stop(PATH_PRU0_0);
    pru_stop(PATH_PRU0_1);

    return 0;
}
