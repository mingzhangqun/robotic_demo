#include "common.h"


uint32_t select_chs(void)
{
    uint32_t chs = 0;
    char str[64];
    char *ptr = NULL;
    printf("Please select pwm chs[0-13]:(0,1,2,3...)\n");
    fgets(str, sizeof(str), stdin);
    if ((str[0] == 'q') || (str[0] == 'Q'))
        return 0;

    ptr = str;
    while (*ptr) {
        int i = atoi(ptr);
        if (i < 32) {
            chs |= ((uint32_t)0x01<<i);
        }

        ptr = strchr(ptr, ',');
        if (NULL == ptr)
            break;
        ptr++;

        if (('\r' == *ptr) || ('\n' == *ptr))
            break;
    }

    return chs;
}


int main(int argc, char *argv[])
{
    uint32_t chs = 0;
    uint32_t duty = 10;
    uint32_t period = 100;
    uint32_t ctrl = 0;

    pru_start(PATH_PRU0_0);
    pru_start(PATH_PRU0_1);
    pwm_init();

    chs = select_chs();
    if (0 == chs)
        goto STOP;

    printf("Selected:0x%04X\n('q' - exit, '+' - inc, '-' - dec, 'r' - run/stop)\n", chs);
    pwm_duty(chs, duty);
    pwm_peroid(chs, period);
    pwm_ctrl(chs, 0x00);
    for (;;) {
        switch (getchar())
        {
        case 'q':
        case 'Q':
            goto STOP;

        case 'r':
        case 'R':
            ctrl = !ctrl;
            pwm_ctrl(chs, ctrl);
            printf("ctrl=%d\n", ctrl);
        break;

        case '+':
            if (duty < period) {
                duty++;
                pwm_duty(chs, duty);
                printf("duty=%d\n", duty);
            }
        break;

        case '-':
            if (duty > 0) {
                duty--;
                pwm_duty(chs, duty);
                printf("duty=%d\n", duty);
            }
        break;
        }
    }

STOP:
    printf("Stopped!!!\n");
    pwm_ctrl(PWM_ALL, 0x00);
    pwm_deinit();
    pru_stop(PATH_PRU0_0);
    pru_stop(PATH_PRU0_1);

    return 0;
}
