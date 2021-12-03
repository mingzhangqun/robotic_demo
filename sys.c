#include "sys.h"


int run_cmd(const char *cmd, char *result, int len)
{
    FILE *fp = NULL;

    fp = popen(cmd, "r");
    if (NULL == fp) {
        printf("Error:popen '%s' failed!\n", cmd);
        return -1;
    }

    if (result)
        fgets(result, len, fp);

    pclose(fp);

    if (result)
        return strlen(result);

    return 0;
}


int pru_start(char *pru)
{
    int i = 0;
    char str[64] = "";

    for (i=0; i<2; i++) {
        sprintf(str, "cat %s/state", pru);
        run_cmd(str, str, 64);
        if (!strcmp(str, "running\n")) {
            printf("running:%s\n", pru);
            break;
        }
        sprintf(str, "echo start > %s/state", pru);
        printf("%s\n", str);
        run_cmd(str, str, 64);
    }
    if (i >= 2) {
        return -1;
    }

    return 0;
}


int pru_stop(char *pru)
{
    int i = 0;
    char str[64] = "";

    for (i=0; i<2; i++) {
        sprintf(str, "cat %s/state", pru);
        run_cmd(str, str, 64);
        if (!strcmp(str, "offline\n")) {
            printf("offline:%s\n", pru);
            break;
        }
        sprintf(str, "echo stop > %s/state", pru);
        printf("%s\n", str);
        run_cmd(str, str, 64);
    }
    if (i >= 2) {
        return -1;
    }

    return 0;
}
