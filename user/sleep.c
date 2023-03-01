/**
 * @file sleep.c
 * @author Tommy Shen(10215501403@stu.ecnu.edu.cn)
 * @brief imply of sleep() function
 * @date 2023.2.27
*/

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: sleep (number of ticks)\n");
        exit(1);
    }
    int ticks = atoi(argv[1]);
    if (!ticks) {
        printf("Usage: sleep (number of ticks, != 0)\n");
        exit(1);
    }
    sleep(ticks);
    exit(0);
}