#include "header.h"

static void create_tasks(void)
{
    for (int i = 0; i < LOTTERY_TASKS; i++)
    {
        int burst = (rand() % BURST_MAX) + 1;

        lottery_task_g[i] = (lottery_task_t){
            .id = i,
            .tickets = (rand() % TICKET_MAX) + 1,
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
    uint16_t arrived_tasks[LOTTERY_TASKS];
    uint16_t arrived_count = 0;

    for (int i = 0; i < LOTTERY_TASKS; i++)
    {
        if (lottery_task_g[i].status == NEW && lottery_task_g[i].arrival_tick <= current_tick)
        {
            lottery_task_g[i].status = READY;
            arrived_tasks[arrived_count++] = i;
        }

        if (lottery_task_g[i].status == FINISHED)
            finished_tasks++;
    }

    // Go through newly arrived tasks
    for (int a = 0; a < arrived_count - 1; a++)
    {
        for (int b = a + 1; b < arrived_count; b++)
        {
            if (lottery_task_g[arrived_tasks[b]].arrival_tick < lottery_task_g[arrived_tasks[a]].arrival_tick)
            {
                int tmp = arrived_tasks[a];
                arrived_tasks[a] = arrived_tasks[b];
                arrived_tasks[b] = tmp;
            }
        }
    }

    for (int a = 0; a < arrived_count; a++)
    {
        lottery_task_t *t = &lottery_task_g[arrived_tasks[a]];
        log_printf("🟣 [TICK %02u]: {arrive} | task %-2d (%2d)                  | [%d ticks]\n",
                   t->arrival_tick, t->id, t->tickets, t->burst_ticks);
    }

    if (finished_tasks == LOTTERY_TASKS)
        return false;

    return true;
}

static int pick_task(void)
{
    int tickets_total = 0;
    int tickets_low[LOTTERY_TASKS] = {0};
    int tickets_high[LOTTERY_TASKS] = {0};

    for (int i = 0; i < LOTTERY_TASKS; i++)
    {
        if (lottery_task_g[i].status == READY)
        {
            tickets_low[i] = tickets_total + 1;
            tickets_total += lottery_task_g[i].tickets;
            tickets_high[i] = tickets_total;
        }
    }

    if (tickets_total == 0)
        return -1;

    int ticket_pick = (rand() % tickets_total) + 1;

    for (int i = 0; i < LOTTERY_TASKS; i++)
    {
        if (lottery_task_g[i].status == READY && ticket_pick <= tickets_high[i])
        {
            log_printf("🟢 [TICK %02u]: {winner} | task %-2d (%3d:%-3d) & (%3d/%-3d) | [%d ticks restantes]\n",
                       current_tick, lottery_task_g[i].id,
                       tickets_low[i], tickets_high[i], ticket_pick, tickets_total,
                       lottery_task_g[i].remaining_ticks);
            return i;
        }
    }

    return -1;
}

static void run_task(int task_id)
{
    lottery_task_t *t = &lottery_task_g[task_id];

    if (t->remaining_ticks <= QUANTUM_TICK)
    {
        wait_ticks(t->remaining_ticks);
        check_tasks();
        t->status = FINISHED;
        t->remaining_ticks = 0;
        t->finished_tick = current_tick;

        int turnaround = t->finished_tick - t->arrival_tick;
        int wait = turnaround - t->burst_ticks;

        log_printf("🔴 [TICK %02u]: {finish} | task %-2d (%2d)                  | [%2d ticks - %2d TAT - %2d WT ]\n",
                   t->finished_tick, t->id, t->tickets, t->burst_ticks, turnaround, wait);
    }
    else
    {
        wait_ticks(QUANTUM_TICK);
        t->remaining_ticks -= QUANTUM_TICK;
    }
}

void lottery_tasks()
{
    int task_id;

    setvbuf(stdout, NULL, _IONBF, 0);

    log_fp = fopen("log.txt", "w");
    if (log_fp == NULL)
        perror("fopen log.txt");

    // Create all tasks with random parameters
    create_tasks();

    log_printf("Simulação escalonador - LOTERIA:\n");

    while (true)
    {
        // Check for new runnable tasks
        if (!check_tasks())
        {
            log_printf("\nTasks de loteria terminaram!\n\r");
            break;
        }

        // Run lottery and find the winner task
        task_id = pick_task();
        if (task_id == -1)
        {
            // Wait until next tick
            wait_ticks(1);
            continue;
        }

        run_task(task_id);
    }
}

void lottery_log()
{
    float avg_turnaround = 0;
    float avg_wait = 0;
    log_printf("===================== LOTTERY SCHEDULER =====================\n");
    log_printf(" ID  | TICKETS | ARRIVAL | BURST | FINISH | TURNAROUND | WAIT\n");
    log_printf("-----|---------|---------|-------|--------|------------|-----\n");
    for (int i = 0; i < LOTTERY_TASKS; i++)
    {
        lottery_task_t *t = &lottery_task_g[i];
        log_printf(" %3d | %7d | %7d | %5d | %6d | %10d | %4d\n",
                   t->id,
                   t->tickets,
                   t->arrival_tick,
                   t->burst_ticks,
                   t->finished_tick,
                   t->finished_tick - t->arrival_tick,
                   t->finished_tick - t->arrival_tick - t->burst_ticks);
        avg_turnaround += t->finished_tick - t->arrival_tick;
        avg_wait += t->finished_tick - t->arrival_tick - t->burst_ticks;
    }
    log_printf("-----|---------|---------|-------|--------|------------|-----\n");
    log_printf(" AVG |         |         |       |        | %10.1f | %4.1f\n",
               avg_turnaround / LOTTERY_TASKS,
               avg_wait / LOTTERY_TASKS);
}