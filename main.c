#include "common.h"


#define FUNC_CHS        0
#define FUNC_CAR        1
#define FUNC_SELECT     FUNC_CAR


#if (FUNC_SELECT == FUNC_CAR)
#define MOTOR_PERIOD    150//3000
#define MOTOR_CHS       (0x0F)

#define SERVO_PERIOD    30000
#define SERVO_CHS       (0x01<<4)
#define SERVO_DEF       1700
#define SERVO_MAX       2050
#define SERVO_MIN       1350

#define LEDS_PERIOD     100
#define LEDS_CHS        (0x01FF<<5)

#define MAX_SPEED       5

static pthread_t tid_key;
static pthread_t tid_remoter;


static int start = 0;
static void *thread_key(void *arg)
{
    struct input_event ev;
    int speed = 0;
    int fd = kbd_init(KBD_KEY_PATH);

    printf("%s\n", __func__);
    if (fd < 0) {
        printf("kbd_init failed: %s\n", KBD_KEY_PATH);
        return 0;
    }

    for (;;) {
        if (kbd_read(fd, &ev) < 0) {
            printf("kbd_read failed!\n");
            continue;
        }

        if (EV_KEY != ev.type) {
            continue;
        }

        switch (ev.code) {
            case BTN_1:
                if (1 == ev.value) {
                    start = !start;
                    printf("start=%d\n", start);
                    motor_state(start);
                    pwm_ctrl(MOTOR_CHS, start);
                    leds_ctrl(start?speed:0);
                    if (start) {
                        speed = 0;
                        pwm_duty(MOTOR_CHS, (MOTOR_PERIOD/5)*(MAX_SPEED-speed));
                    }
                }
            break;

            case BTN_2:
                if (1 == ev.value) {
                    if (++speed >= (MAX_SPEED+1)) {
                        speed = 0;
                    }
                    printf("start=%d, speed=%d\n", start, (MAX_SPEED-speed));
                    pwm_duty(MOTOR_CHS, (MOTOR_PERIOD/5)*(MAX_SPEED-speed));
                    leds_ctrl(speed);
                }
            break;
        }
    }

    return 0;
}

static void *thread_remoter(void *arg)
{
    struct input_event ev;
    int motor_duty = 0;
    int servo_duty = SERVO_DEF;
    int fd = 0;//kbd_init(KBD_REMOTER_PATH);

    printf("%s\n", __func__);

    for (;;) {
        fd = kbd_init(KBD_REMOTER_PATH);
        if (fd > 0)
            break;
        printf("kbd_init failed: %s\n", KBD_REMOTER_PATH);
        sleep(2);
    }

    for (;;) {
        if (kbd_read(fd, &ev) < 0) {
            printf("kbd_read failed!\n");
            sleep(2);
            fd = kbd_init(KBD_REMOTER_PATH);
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
                    motor_duty = MOTOR_PERIOD;
                    pwm_duty(MOTOR_CHS, motor_duty);
                    pwm_ctrl(MOTOR_CHS, 0x0);
                    motor_state(0);
                    printf("stop!\n\n");
                }
                else if (1 == ev.value) {
                    motor_duty = 0;
                    pwm_duty(MOTOR_CHS, motor_duty);
                    pwm_ctrl(MOTOR_CHS, 0x01);
                    motor_state(1);
                    printf("run = %d\n", motor_duty);
                }
                else if (2 == ev.value) {
                    if (motor_duty > 0) {
                        motor_duty -= 200;
                    }
                    pwm_duty(MOTOR_CHS, motor_duty);
                    printf("burst = %d\n", motor_duty);
                }
            break;

            case KEY_DOWN:
            case KEY_S:
                printf("dn -- ");
                if (0 == ev.value) {
                    motor_duty = MOTOR_PERIOD;
                    pwm_duty(MOTOR_CHS, motor_duty);
                    pwm_ctrl(MOTOR_CHS, 0x0);
                    motor_state(0);
                    printf("stop!\n\n");
                }
                else if (1 == ev.value) {
                    motor_duty = 0;
                    pwm_duty(MOTOR_CHS, motor_duty);
                    pwm_ctrl(MOTOR_CHS, 0x01);
                    motor_state(2);
                    printf("run = %d\n", motor_duty);
                }
                else if (2 == ev.value) {
                    if (motor_duty > 0) {
                        motor_duty -= 200;
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
                    servo_duty = SERVO_MIN;
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
                    servo_duty = SERVO_MAX;
                    pwm_duty(SERVO_CHS, servo_duty);
                    printf("burst = %d\n", servo_duty);
                }
                pwm_ctrl(SERVO_CHS, 0x1);
            break;

            /*case KEY_ESC:
                goto STOP;*/
        }
    }

    return 0;
}


int main(int argc, char *argv[])
{
    struct input_event ev;

    printf("robotic %s\n", __func__);

    if (pru_start(PATH_PRU0_0) < 0)
        goto STOP;
    if (pru_start(PATH_PRU0_1) < 0)
        goto STOP;
    if (pru_start(PATH_PRU1_0) < 0)
        goto STOP;
    if (pru_start(PATH_PRU1_1) < 0)
        goto STOP;
    if (leds_init() < 0)
        goto STOP;
    if (pwm_init() < 0)
        goto STOP;

    // motor
    motor_init();
    pwm_duty(MOTOR_CHS, MOTOR_PERIOD);
    pwm_peroid(MOTOR_CHS, MOTOR_PERIOD);
    pwm_ctrl(MOTOR_CHS, 0x0);

    // servo
    pwm_duty(SERVO_CHS, SERVO_DEF);
    pwm_peroid(SERVO_CHS, SERVO_PERIOD);
    pwm_ctrl(SERVO_CHS, 0x1);

    // LEDS
    pwm_duty(LEDS_CHS, LEDS_PERIOD/2);
    pwm_peroid(LEDS_CHS, LEDS_PERIOD);
    pwm_ctrl(LEDS_CHS, 0x1);

    if (pthread_create(&tid_key, NULL, thread_key, NULL) < 0) {
		printf("Failed: create thread_key!\n");
        goto STOP;
    }

    if (pthread_create(&tid_remoter, NULL, thread_remoter, NULL) < 0) {
		printf("Failed: create thread_remoter!\n");
        goto STOP;
    }

    for (;;) {
        if (!start) {
            sleep(1);
            continue;
        }

        if (start) {
            pwm_duty(LEDS_CHS, 0);
            sleep(2);
            pwm_duty(LEDS_CHS, LEDS_PERIOD/2);
            sleep(2);
        }
    }

STOP:
    printf("Stopped!!!\n");
    leds_deinit();
    motor_deinit();
    pwm_ctrl(PWM_ALL, 0x00);
    pwm_deinit();
    pru_stop(PATH_PRU0_0);
    pru_stop(PATH_PRU0_1);
    pru_stop(PATH_PRU1_0);
    pru_stop(PATH_PRU1_1);
}

#else

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

#endif
