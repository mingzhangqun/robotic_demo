#include "pwm.h"


#define PATH_MEM        "/dev/mem"

static uint32_t *pru_addr = MAP_FAILED;


static const uint32_t pwms[PWM_MAX_CHS][2] = {
    {0, 2}, // 0_5
    {0, 4}, // 0_13
    {0, 5}, // 0_16
    {0, 6}, // 0_17
    {1, 5}, // 1_15
    {1, 2}, // 1_12
    {1, 4}, // 1_14
    {0, 0}, // 0_2
    {1, 3}, // 1_13
    {0, 3}, // 0_8
    {1, 1}, // 1_4
    {1, 0}, // 1_2
    {1, 6}, // 1_16
    {0, 1}, // 0_4
};

int pwm_init(void)
{
    int fd;

    // mmap
    fd = open(PATH_MEM, O_RDWR | O_SYNC);
    if (-1 == fd) {
        printf("Error: open '%s' failed!\n", PATH_MEM);
        return -11;
    }

    pru_addr = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU0_ADDR);
    if (MAP_FAILED == pru_addr) {
        printf("Error: mmap failed!\n");
        return -1;
    }
    close(fd);

    return 0;
}

int pwm_deinit(void)
{
    if (MAP_FAILED == pru_addr) {
        printf("Invalid pru_addr!\n");
        return -1;
    }

    if (munmap(pru_addr, PRU_LEN)) {
        printf("Error: munmap failed!\n");
        return -1;
    }
    else {
        printf("Munmap succeeded!\n");
    }

    return 0;
}

int pwm_ctrl(uint32_t chs, uint32_t ctrl)
{
    int i = 0;
    pwm_t *pwm = NULL;

    if (MAP_FAILED == pru_addr) {
        printf("Invalid pru_addr!\n");
        return -1;
    }

    for (i=0; i<PWM_MAX_CHS; i++) {
        if (!(chs & (0x01<<i)))
            continue;
    
        if (0 == pwms[i][0]) {
            pwm = (pwm_t*)(pru_addr+(PWM0_OFFSET>>2));
        }
        else {
            pwm = (pwm_t*)(pru_addr+(PWM1_OFFSET>>2));
        }

        pwm[pwms[i][1]].ctrl = ctrl;
        pwm[pwms[i][1]].count = 0;
    }

    return 0;
}

int pwm_duty(uint32_t chs, uint32_t duty)
{
    int i = 0;
    pwm_t *pwm = NULL;

    if (MAP_FAILED == pru_addr) {
        printf("Invalid pru_addr!\n");
        return -1;
    }

    for (i=0; i<PWM_MAX_CHS; i++) {
        if (!(chs & (0x01<<i)))
            continue;
    
        if (0 == pwms[i][0]) {
            pwm = (pwm_t*)(pru_addr+(PWM0_OFFSET>>2));
        }
        else {
            pwm = (pwm_t*)(pru_addr+(PWM1_OFFSET>>2));
        }

        pwm[pwms[i][1]].duty = duty;
    }

    return 0;
}

int pwm_peroid(uint32_t chs, uint32_t period)
{
    int i = 0;
    pwm_t *pwm = NULL;

    if (MAP_FAILED == pru_addr) {
        printf("Invalid pru_addr!\n");
        return -1;
    }

    for (i=0; i<PWM_MAX_CHS; i++) {
        if (!(chs & (0x01<<i)))
            continue;
    
        if (0 == pwms[i][0]) {
            pwm = (pwm_t*)(pru_addr+(PWM0_OFFSET>>2));
        }
        else {
            pwm = (pwm_t*)(pru_addr+(PWM1_OFFSET>>2));
        }

        pwm[pwms[i][1]].period = period;
    }

    return 0;
}