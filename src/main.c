#include "header.h"

lottery_task_t *lottery_task_g = NULL;
priority_task_t *priority_task_g = NULL;

int main(void)
{
    srand((unsigned int)time(NULL));

    lottery_task_g = malloc(LOTTERY_TASKS * sizeof(lottery_task_t));
    priority_task_g = malloc(PRIORITY_TASKS * sizeof(priority_task_t));

    usleep(10 * TICK_TO_US);
    return 0;
}