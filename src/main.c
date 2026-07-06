#include "header.h"

int main(void)
{
    srand(1234);

    lottery_task_g = malloc(LOTTERY_TASKS * sizeof(lottery_task_t));
    priority_task_g = malloc(PRIORITY_TASKS * sizeof(priority_task_t));

    /* Priority Scheduler */
    priority_tasks();
    priority_log();
    current_tick = 0;

    /* Lottery Scheduler */
    lottery_tasks();
    lottery_log();

    return 0;
}