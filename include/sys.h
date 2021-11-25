#ifndef __SYS_H__
#define __SYS_H__

#include "common.h"


#define PATH_PRU0_0     "/dev/remoteproc/j7-pru0_0"
#define PATH_PRU0_1     "/dev/remoteproc/j7-pru0_1"
#define PATH_PRU1_0     "/dev/remoteproc/j7-pru1_0"
#define PATH_PRU1_1     "/dev/remoteproc/j7-pru1_1"

int run_cmd(const char *cmd, char *result, int len);
int pru_start(char *pru);
int pru_stop(char *pru);

#endif // __SYS_H__
