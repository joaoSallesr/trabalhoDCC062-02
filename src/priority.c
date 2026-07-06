#include "header.h"

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
    uint16_t arrived_tasks[PRIORITY_TASKS];
    uint16_t arrived_count = 0;

    for (int i = 0; i < PRIORITY_TASKS; i++)
    {
        if (priority_task_g[i].status == NEW && priority_task_g[i].arrival_tick <= current_tick)
        {
            priority_task_g[i].status = READY;
            arrived_tasks[arrived_count++] = i;
        }

        if (priority_task_g[i].status == FINISHED)
            finished_tasks++;
    }

    // Go through newly arrived tasks
    for (int a = 0; a < arrived_count - 1; a++)
    {
        for (int b = a + 1; b < arrived_count; b++)
        {
            if (priority_task_g[arrived_tasks[b]].arrival_tick < priority_task_g[arrived_tasks[a]].arrival_tick)
            {
                int tmp = arrived_tasks[a];
                arrived_tasks[a] = arrived_tasks[b];
                arrived_tasks[b] = tmp;
            }
        }
    }

    for (int a = 0; a < arrived_count; a++)
    {
        priority_task_t *t = &priority_task_g[arrived_tasks[a]];
        log_printf("🟣 [TICK %02u]: {arrive} | task %-2d (%2d*)                   | [%2d ticks]\n",
                   t->arrival_tick, t->id, t->priority, t->burst_ticks);
    }

    if (finished_tasks == PRIORITY_TASKS)
        return false;

    return true;
}

static int pick_task(bool *was_unblocked)
{
    *was_unblocked = false;

    for (int p = 1; p <= PRIORITY_MAX; p++)
    {
        bool unblock_tasks = false;

        // Go through every task with priority 'p'
        for (int i = 0; i < PRIORITY_TASKS; i++)
        {
            if (priority_task_g[i].status == READY && priority_task_g[i].priority == p)
            {
                priority_task_g[i].status = PREEMPTED;
                return i;
            }
        }

        // If it can't find a READY task with priority 'p' checks for PREEMPTED tasks
        for (int i = 0; i < PRIORITY_TASKS; i++)
        {
            if (priority_task_g[i].status == PREEMPTED && priority_task_g[i].priority == p)
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
                if (priority_task_g[i].status == PREEMPTED && priority_task_g[i].priority == p)
                    priority_task_g[i].status = READY;
            }

            for (int i = 0; i < PRIORITY_TASKS; i++)
            {
                if (priority_task_g[i].status == READY && priority_task_g[i].priority == p)
                {
                    priority_task_g[i].status = PREEMPTED;
                    *was_unblocked = true;
                    return i;
                }
            }
        }
    }

    return -1;
}

static void run_task(int task_id)
{
    priority_task_t *t = &priority_task_g[task_id];

    if (t->remaining_ticks <= QUANTUM_TICK)
    {
        wait_ticks(t->remaining_ticks);
        t->status = FINISHED;
        t->remaining_ticks = 0;
        t->finished_tick = current_tick;

        int turnaround = t->finished_tick - t->arrival_tick;
        int wait = turnaround - t->burst_ticks;

        log_printf("🔴 [TICK %02u]: {finish} | task %-2d (%2d*)                   | [%2d ticks - %2d TAT - %2d WT ]\n",
                   t->finished_tick, t->id, t->priority, t->burst_ticks, turnaround, wait);
    }
    else
    {
        wait_ticks(QUANTUM_TICK);
        t->remaining_ticks -= QUANTUM_TICK;
    }
}

static void log_tick(int task_id, int last_task_id, bool last_task_finished, bool was_unblocked)
{
    priority_task_t *t = &priority_task_g[task_id];

    if (task_id == last_task_id)
    {
        // log_printf("[TICK %02u]: task %d (%d*) rodando [%d ticks restantes]\n", current_tick, t->id, t->priority, t->remaining_ticks);
        return;
    }

    if (last_task_id != -1 && !last_task_finished)
    {
        priority_task_t *prev = &priority_task_g[last_task_id];

        if (prev->priority == t->priority)
            log_printf("🔵 [TICK %02u]: {rotate} | task %-2d (%2d*) <-> task %-2d (%2d*) | [%2d ticks / %2d ticks]\n",
                       current_tick, prev->id, prev->priority, t->id, t->priority,
                       prev->remaining_ticks, t->remaining_ticks);
        else if (prev->priority > t->priority)
            log_printf("🟤 [TICK %02u]: {swap}   | task %-2d (%2d*) --> task %-2d (%2d*) | [%2d ticks / %2d ticks]\n",
                       current_tick, prev->id, prev->priority, t->id, t->priority,
                       prev->remaining_ticks, t->remaining_ticks);

        return;
    }

    if (last_task_id == -1)
    {
        log_printf("🟢 [TICK %02u]: {start}  | task %-2d (%2d*)                   | [%2d ticks restantes]\n",
                   current_tick, t->id, t->priority, t->remaining_ticks);
    }
    else if (was_unblocked)
    {
        log_printf("🟠 [TICK %02u]: {return} | task %-2d (%2d*)                   | [%2d ticks restantes]\n",
                   current_tick, t->id, t->priority, t->remaining_ticks);
    }
    else
    {
        log_printf("🟡 [TICK %02u]: {select} | task %-2d (%2d*)                   | [%2d ticks restantes]\n",
                   current_tick, t->id, t->priority, t->remaining_ticks);
    }
}

void priority_tasks()
{
    int task_id;
    int last_task_id = -1;
    bool last_task_finished = false;

    setvbuf(stdout, NULL, _IONBF, 0);

    log_fp = fopen("log.txt", "w");
    if (log_fp == NULL)
        perror("fopen log.txt");

    // Create all tasks with random parameters
    create_tasks();

    log_printf("Simulação escalonador - PRIORIDADE:\n");

    while (true)
    {
        // Check for new runnable tasks
        if (!check_tasks())
        {
            log_printf("\nTasks de prioridade terminaram!\n\r");
            break;
        }

        // Find a READY task with the highest priority possible
        bool was_unblocked = false;
        task_id = pick_task(&was_unblocked);
        if (task_id == -1)
        {
            // Wait until next tick
            wait_ticks(1);
            continue;
        }

        log_tick(task_id, last_task_id, last_task_finished, was_unblocked);

        // Run selected task
        run_task(task_id);

        last_task_finished = (priority_task_g[task_id].status == FINISHED);
        last_task_id = task_id;
    }
}

void priority_log()
{
    float avg_turnaround = 0;
    float avg_wait = 0;

    log_printf("=================== PRIORITY SCHEDULER ===================\n");
    log_printf(" ID  | PRIO | ARRIVAL | BURST | FINISH | TURNAROUND | WAIT\n");
    log_printf("-----|------|---------|-------|--------|------------|-----\n");

    for (int i = 0; i < PRIORITY_TASKS; i++)
    {
        priority_task_t *t = &priority_task_g[i];
        log_printf(" %3d | %4d | %7d | %5d | %6d | %10d | %4d\n",
                   t->id,
                   t->priority,
                   t->arrival_tick,
                   t->burst_ticks,
                   t->finished_tick,
                   t->finished_tick - t->arrival_tick,
                   t->finished_tick - t->arrival_tick - t->burst_ticks);

        avg_turnaround += t->finished_tick - t->arrival_tick;
        avg_wait += t->finished_tick - t->arrival_tick - t->burst_ticks;
    }

    log_printf("-----|------|---------|-------|--------|------------|-----\n");
    log_printf(" AVG |      |         |       |        | %10.1f | %4.1f\n",
               avg_turnaround / PRIORITY_TASKS,
               avg_wait / PRIORITY_TASKS);
}