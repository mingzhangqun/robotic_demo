#ifndef __LEDS_H__
#define __LEDS_H__

#include "common.h"


int leds_init(void);
int leds_deinit(void);
int leds_ctrl(uint32_t leds);


#endif // __LEDS_H__
