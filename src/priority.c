#include "header.h"

uint32_t current_tick;

void wait_ticks(int ticks)
{
    current_tick += ticks;
    usleep(ticks * TICK_TO_US);
}

static void create_tasks(void)
{
    for (int i = 0; i < PRIORITY_TASKS; i++)
    {
        int burst = (rand() % BURST_MAX) + 1;

        priority_task_g[i] = (priority_task_t){
            .id = i,
            .priority = (rand() % PRIORITY_MAX) + 1,
            .arrival_tick = rand() % ARRIVAL_MAX,
            .burst_ticks = burst,
            .remaining_ticks = burst,
            .status = NEW,
        };
    }
}

static bool check_tasks(void)
{
    uint16_t finished_tasks = 0;

    for (int i = 0; i < PRIORITY_TASKS; i++)
    {
        if (priority_task_g[i].status == NEW && priority_task_g[i].arrival_tick <= current_tick)
            priority_task_g[i].status = READY;

        if (priority_task_g[i].status == FINISHED)
            finished_tasks++;
    }

    if (finished_tasks == PRIORITY_TASKS)
        return false;

    return true;
}

static int pick_task(void)
{
    bool unblock_tasks = false;

    for (int p = 1; p <= PRIORITY_MAX; p++)
    {
        // Go through every task with priority 'p'
        for (int i = 0; i < PRIORITY_TASKS; i++)
        {
            if (priority_task_g[i].status == READY && priority_task_g[i].priority == p)
            {
                priority_task_g[i].status = BLOCKED;
                return i;
            }
        }

        // If it can't find a READY task with priority 'p' checks for BLOCKED tasks
        for (int i = 0; i < PRIORITY_TASKS; i++)
        {
            if (priority_task_g[i].status == BLOCKED && priority_task_g[i].priority == p)
            {
                unblock_tasks = true;
                break;
            }
        }

        // Unblock tasks with priority 'p' and return the first READY task
        if (unblock_tasks)
        {
            for (int i = 0; i < PRIORITY_TASKS; i++)
            {
                if (priority_task_g[i].status == BLOCKED && priority_task_g[i].priority == p)
                    priority_task_g[i].status = READY;
            }

            for (int i = 0; i < PRIORITY_TASKS; i++)
            {
                if (priority_task_g[i].status == READY && priority_task_g[i].priority == p)
                {
                    priority_task_g[i].status = BLOCKED;
                    return i;
                }
            }
        }
    }

    return -1;
}

static void run_task(int task_id)
{
    if (priority_task_g[task_id].remaining_ticks < QUANTUM_TICK)
    {
        priority_task_g[task_id].finished_tick = current_tick;
        priority_task_g[task_id].remaining_ticks = 0;
        priority_task_g[task_id].status = FINISHED;

        wait_ticks(priority_task_g[task_id].remaining_ticks);
    }
    else
    {
        priority_task_g[task_id].remaining_ticks -= QUANTUM_TICK;

        wait_ticks(QUANTUM_TICK);
    }
}

void priority_tasks()
{
    int task_id;

    // Create all tasks with random parameters
    create_tasks();

    while (true)
    {
        // Check for new runnable tasks
        if (!check_tasks())
        {
            printf("All priority tasks finished!\n\r");
            break;
        }

        // Find a READY task with the highest priority possible
        task_id = pick_task();
        if (task_id == -1)
        {
            // Wait until next tick
            wait_ticks(1);
            continue;
        }

        // Run selected task
        run_task(task_id);
    }
}

void priority_log()
{
    printf("========== PRIORITY SCHEDULER ==========\n\r");
    for (int i = 0; i < PRIORITY_TASKS; i++)
    {
        printf("Task [%d] ==========\n\r", i);
        printf("- Task priority: %d\n\r", priority_task_g[i].priority);
        printf("- Task arrival time   : %d\n\r", priority_task_g[i].arrival_tick);
        printf("- Task burst time     : %d ticks\n\r", priority_task_g[i].burst_ticks);
        printf("- Task finish time    : %d\n\r", priority_task_g[i].finished_tick);
        printf("- Task turnaround time: %d\n\r", priority_task_g[i].finished_tick - priority_task_g[i].arrival_tick);
        printf("- Task wait time      : %d ticks\n\r", priority_task_g[i].finished_tick - priority_task_g[i].arrival_tick - priority_task_g[i].burst_ticks);
    }
}