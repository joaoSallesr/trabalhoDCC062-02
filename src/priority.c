#include "header.h"

void create_tasks()
{
    int task_id = 0;
    for (int i = 0; i < PRIORITY_TASKS; i++)
    {
        priority_task_g[i] = (priority_task_t){
            .id = task_id,
            .priority = (rand() % PRIORITY_MAX) + 1,
            .arrival_time = (rand() % BURST_MAX) + 0,
            .burst_time = (rand() % BURST_MAX) + 1,
            .remaining_time = priority_task_g[i].burst_time,
            .status = NEW,
        };

        task_id++;
    }
}

void check_tasks() {}

void choose_task() {}

void run_task(int task_id)
{
    if (priority_task_g[task_id].remaining_time < QUANTUM_TICK)
    {
        priority_task_g[task_id].remaining_time = 0;
        priority_task_g[task_id].status = FINISHED;
    }
    else
    {
        priority_task_g[task_id].remaining_time -= QUANTUM_TICK;
    }

    usleep(QUANTUM_TICK * TICK_TO_US);
}