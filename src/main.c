#include "header.h"

int main(void)
{
    srand(1234);

    lottery_task_g = malloc(LOTTERY_TASKS * sizeof(lottery_task_t));
    priority_task_g = malloc(PRIORITY_TASKS * sizeof(priority_task_t));

    switch (menu())
    {
    case 1:
        priority_tasks();
        priority_log();
        break;

    case 2:
        lottery_tasks();
        lottery_log();
        break;

    case 3:
        priority_tasks();
        priority_log();

        current_tick = 0;

        lottery_tasks();
        lottery_log();
        break;

    case 0:
        printf("saindo...\n");
        break;
    }

    free(priority_task_g);
    free(lottery_task_g);

    return 0;
}