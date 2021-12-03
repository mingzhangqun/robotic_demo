#include "common.h"


#if 0
uint32_t select_chs(void)
{
    uint32_t chs = 0;
    char str[64];
    char *ptr = NULL;
    printf("Please select pwm chs[0-%d]:(0,1,2,3...)\n", PWM_MAX_CHS-1);
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
    uint32_t duty = 0;
    uint32_t period = 3000;
    uint32_t ctrl = 0;

    pru_start(PATH_PRU0_0);
    pru_start(PATH_PRU0_1);
    pru_start(PATH_PRU1_0);
    pru_start(PATH_PRU1_1);
    pwm_init();

    chs = select_chs();
    if (0 == chs)
        goto STOP;

    printf("Selected:0x%04X\n('q' - exit, '+' - inc, '-' - dec, 'r' - run/stop)\n", chs);
    pwm_duty(chs, duty);
    pwm_peroid(chs, period);
    pwm_ctrl(chs, 0x00);

    motor_init();
    motor_state(2);

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
                duty+=100;
                pwm_duty(chs, duty);
                printf("duty=%d\n", duty);
            }
        break;

        case '-':
            if (duty > 0) {
                duty-=100;
                pwm_duty(chs, duty);
                printf("duty=%d\n", duty);
            }
        break;
        }
    }

STOP:
    printf("Stopped!!!\n");
    motor_deinit();
    pwm_ctrl(PWM_ALL, 0x00);
    pwm_deinit();
    pru_stop(PATH_PRU0_0);
    pru_stop(PATH_PRU0_1);
    pru_stop(PATH_PRU1_0);
    pru_stop(PATH_PRU1_1);

    return 0;
}
#else

#define MOTOR_PERIOD    3000
#define MOTOR_CHS       (0x01<<0)

#define SERVO_PERIOD    30000
#define SERVO_CHS       (0x01<<4)
#define SERVO_DEF       2100
#define SERVO_MAX       2600
#define SERVO_MIN       1600

int main(int argc, char *argv[])
{
    struct input_event ev;
    int motor_duty = 0;
    int servo_duty = SERVO_DEF;

    if (pru_start(PATH_PRU0_0) < 0)
        goto STOP;
    if (pru_start(PATH_PRU0_1) < 0)
        goto STOP;
    if (pru_start(PATH_PRU1_0) < 0)
        goto STOP;
    if (pru_start(PATH_PRU1_1) < 0)
        goto STOP;
    if (kbd_init() < 0)
        goto STOP;
    if (pwm_init() < 0)
        goto STOP;

    // motor
    motor_init();
    pwm_duty(MOTOR_CHS, motor_duty);
    pwm_peroid(MOTOR_CHS, MOTOR_PERIOD);
    pwm_ctrl(MOTOR_CHS, 0x0);

    // servo
    pwm_duty(SERVO_CHS, SERVO_DEF);
    pwm_peroid(SERVO_CHS, SERVO_PERIOD);
    pwm_ctrl(SERVO_CHS, 0x1);

    for (;;) {
        if (kbd_read(&ev) < 0) {
            printf("kbd_read failed!\n");
            continue;
        }

        if (EV_KEY != ev.type) {
            //printf("ev.type=0x%x\n", ev.type);
            continue;
        }

        switch (ev.code) {
        case KEY_UP:
        case KEY_W:
            printf("up -- ");
            if (0 == ev.value) {
                motor_duty = 0;
                pwm_duty(MOTOR_CHS, motor_duty);
                pwm_ctrl(MOTOR_CHS, 0x0);
                motor_state(0);
                printf("stop!\n\n");
            }
            else if (1 == ev.value) {
                motor_duty = 1000;
                pwm_duty(MOTOR_CHS, motor_duty);
                pwm_ctrl(MOTOR_CHS, 0x01);
                motor_state(1);
                printf("run = %d\n", motor_duty);
            }
            else if (2 == ev.value) {
                if (motor_duty < MOTOR_PERIOD) {
                    motor_duty += 200;
                }
                pwm_duty(MOTOR_CHS, motor_duty);
                printf("burst = %d\n", motor_duty);
            }
        break;

        case KEY_DOWN:
        case KEY_S:
            printf("dn -- ");
            if (0 == ev.value) {
                motor_duty = 0;
                pwm_duty(MOTOR_CHS, motor_duty);
                pwm_ctrl(MOTOR_CHS, 0x0);
                motor_state(0);
                printf("stop!\n\n");
            }
            else if (1 == ev.value) {
                motor_duty = 1000;
                pwm_duty(MOTOR_CHS, motor_duty);
                pwm_ctrl(MOTOR_CHS, 0x01);
                motor_state(2);
                printf("run = %d\n", motor_duty);
            }
            else if (2 == ev.value) {
                if (motor_duty < MOTOR_PERIOD) {
                    motor_duty += 200;
                }
                pwm_duty(MOTOR_CHS, motor_duty);
                printf("burst = %d\n", motor_duty);
            }
        break;

        case KEY_LEFT:
        case KEY_A:
            printf("lt -- ");
            if (0 == ev.value) {
                servo_duty = SERVO_DEF;
                pwm_duty(SERVO_CHS, servo_duty);
                printf("stop!\n\n");
            }
            else {
                /*if (servo_duty < SERVO_MAX) {
                    servo_duty += 100;
                }*/
                servo_duty = SERVO_MAX;
                pwm_duty(SERVO_CHS, servo_duty);
                printf("burst = %d\n", servo_duty);
            }
            pwm_ctrl(SERVO_CHS, 0x1);
        break;

        case KEY_RIGHT:
        case KEY_D:
            printf("rt -- ");
            if (0 == ev.value) {
                servo_duty = SERVO_DEF;
                pwm_duty(SERVO_CHS, servo_duty);
                printf("stop!\n\n");
            }
            else {
                /*if (servo_duty > SERVO_MIN) {
                    servo_duty -= 100;
                }*/
                servo_duty = SERVO_MIN;
                pwm_duty(SERVO_CHS, servo_duty);
                printf("burst = %d\n", servo_duty);
            }
            pwm_ctrl(SERVO_CHS, 0x1);
        break;

        case KEY_ESC:
            goto STOP;
        }
    }

STOP:
    printf("Stopped!!!\n");
    motor_deinit();
    pwm_ctrl(PWM_ALL, 0x00);
    pwm_deinit();
    kbd_deinit();
    pru_stop(PATH_PRU0_0);
    pru_stop(PATH_PRU0_1);
    pru_stop(PATH_PRU1_0);
    pru_stop(PATH_PRU1_1);
}

#endif
