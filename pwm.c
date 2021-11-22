#include "pwm.h"


#define PATH_MEM        "/dev/mem"

static uint32_t *pru = MAP_FAILED;

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

#define ASSERT_PRU(pru)     if (MAP_FAILED == pru) {printf("Invalid pru_addr!\n"); return -1;}
#define PWM_CHS(i)          (pwms[i][0]?(pwm_t*)(pru+(PWM1_OFFSET>>2)):(pwm_t*)(pru+(PWM0_OFFSET>>2)))[pwms[i][1]]

int pwm_init(void)
{
    int fd;

    // mmap
    fd = open(PATH_MEM, O_RDWR | O_SYNC);
    if (-1 == fd) {
        printf("Error: open '%s' failed!\n", PATH_MEM);
        return -11;
    }

    pru = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU0_ADDR);
    ASSERT_PRU(pru);

    close(fd);

    return 0;
}

int pwm_deinit(void)
{
    ASSERT_PRU(pru);
    if (munmap(pru, PRU_LEN)) {
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

    ASSERT_PRU(pru);
    for (i=0; i<PWM_MAX_CHS; i++) {
        if (!(chs & (0x01<<i)))
            continue;

        PWM_CHS(i).ctrl = ctrl;
        PWM_CHS(i).count = 0;
    }

    return 0;
}

int pwm_duty(uint32_t chs, uint32_t duty)
{
    int i = 0;

    ASSERT_PRU(pru);
    for (i=0; i<PWM_MAX_CHS; i++) {
        if (!(chs & (0x01<<i)))
            continue;

        PWM_CHS(i).duty = duty;
    }

    return 0;
}

int pwm_peroid(uint32_t chs, uint32_t period)
{
    int i = 0;

    ASSERT_PRU(pru);
    for (i=0; i<PWM_MAX_CHS; i++) {
        if (!(chs & (0x01<<i)))
            continue;

        PWM_CHS(i).period = period;
    }

    return 0;
}
