#ifndef __PWM_H__
#define __PWM_H__

#include "common.h"


typedef struct {
	uint32_t ctrl;
	uint32_t duty;
	uint32_t period;
	uint32_t count;
}pwm_t;


#define PRU0_ADDR       0xb000000
#define PRU1_ADDR       0xb100000
#define PRU_LEN         0x80000
#define PWM0_OFFSET     0x10000
#define PWM1_OFFSET     0x10100

#define PWM_MAX_CHS     15
#define PWM_CH0         (0x01 << 0)
#define PWM_CH1         (0x01 << 1)
#define PWM_CH2         (0x01 << 2)
#define PWM_CH3         (0x01 << 3)
#define PWM_CH4         (0x01 << 4)
#define PWM_CH5         (0x01 << 5)
#define PWM_CH6         (0x01 << 6)
#define PWM_CH7         (0x01 << 7)
#define PWM_CH8         (0x01 << 8)
#define PWM_CH9         (0x01 << 9)
#define PWM_CH10        (0x01 << 10)
#define PWM_CH11        (0x01 << 11)
#define PWM_CH12        (0x01 << 12)
#define PWM_CH13        (0x01 << 13)
#define PWM_ALL         0xFFFF


int pwm_init(void);
int pwm_deinit(void);
int pwm_ctrl(uint32_t chs, uint32_t ctrl);
int pwm_duty(uint32_t chs, uint32_t duty);
int pwm_peroid(uint32_t chs, uint32_t period);


#endif // __PWM_H__
