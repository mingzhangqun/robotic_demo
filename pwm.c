#include "pwm.h"


#define PATH_MEM        "/dev/mem"

static uint32_t *pru0 = MAP_FAILED;
static uint32_t *pru1 = MAP_FAILED;

static const uint32_t pwms[PWM_MAX_CHS][2] = {
    {0, 3}, // 0_5
    {0, 0}, // 0_1
    {0, 5}, // 0_16
    {0, 6}, // 0_17
    {1, 5}, // 1_15
    {1, 2}, // 1_12
    {1, 4}, // 1_14
    {0, 1}, // 0_2
    {1, 3}, // 1_13
    {0, 4}, // 0_8
    {1, 1}, // 1_4
    {1, 0}, // 1_2
    {1, 6}, // 1_16
    {0, 2}, // 0_4
    {2, 3}, // 0_5
};

//#define ASSERT_PRU(cond)     if (cond) { printf("Invalid pru_addr!\n"); return -1;}
//#define PWM_CHS(i)          (pwms[i][0]?(pwm_t*)(pru+(PWM1_OFFSET>>2)):(pwm_t*)(pru+(PWM0_OFFSET>>2)))[pwms[i][1]]

int pwm_init(void)
{
    int fd;
    int ret = 0;

    // mmap
    fd = open(PATH_MEM, O_RDWR | O_SYNC);
    if (-1 == fd) {
        printf("Error: open '%s' failed!\n", PATH_MEM);
        return -11;
    }

    pru0 = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU0_ADDR);
    pru1 = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU1_ADDR);
    close(fd);

    if (MAP_FAILED == pru0) {
        printf("Error: mmap pru0 failed!\n");
        ret = -1;
    }

    if (MAP_FAILED == pru1) {
        printf("Error: mmap pru1 failed!\n");
        ret = -1;
    }

    return ret;
}

int pwm_deinit(void)
{
    int ret = 0;

    if (MAP_FAILED != pru0) {
        if (munmap(pru0, PRU_LEN)) {
            printf("Error: munmap pru0 failed!\n");
            ret = -1;
        }
    }

    if (MAP_FAILED != pru1) {
        if (munmap(pru1, PRU_LEN)) {
            printf("Error: munmap pru1 failed!\n");
            ret = -1;
        }
    }

    if (!ret) {
        printf("munmap succeeded!\n");
    }

    return ret;
}

pwm_t *pwm_get(uint32_t ch)
{
    pwm_t *pwm = NULL;
    uint32_t *pru = pru0;
    uint32_t offset = PWM0_OFFSET;

    if (pwms[ch][0] >= 2) {
        pru = pru1;
    }
    if (MAP_FAILED == pru) {
        return NULL;
    }

    if (pwms[ch][0] & 0x01) {
        offset = PWM1_OFFSET;
    }

    pwm = &((pwm_t*)(pru+(offset>>2)))[pwms[ch][1]];

    return pwm;
}

int pwm_ctrl(uint32_t chs, uint32_t ctrl)
{
    int i = 0;
    pwm_t *pwm = NULL;

    for (i=0; i<PWM_MAX_CHS; i++) {
        if (!(chs & (0x01<<i)))
            continue;

        pwm = pwm_get(i);
        if (pwm) {
            pwm->ctrl = ctrl;
            pwm->count = 0;
        }
    }

    return 0;
}

int pwm_duty(uint32_t chs, uint32_t duty)
{
    int i = 0;
    pwm_t *pwm = NULL;

    for (i=0; i<PWM_MAX_CHS; i++) {
        if (!(chs & (0x01<<i)))
            continue;

        pwm = pwm_get(i);
        if (pwm) {
            pwm->duty = duty;
        }
    }

    return 0;
}

int pwm_peroid(uint32_t chs, uint32_t period)
{
    int i = 0;
    pwm_t *pwm = NULL;

    for (i=0; i<PWM_MAX_CHS; i++) {
        if (!(chs & (0x01<<i)))
            continue;

        pwm = pwm_get(i);
        if (pwm) {
            pwm->period = period;
        }
    }

    return 0;
}
