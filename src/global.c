#include "header.h"

FILE *log_fp = NULL;
uint32_t current_tick = 0;

lottery_task_t *lottery_task_g = NULL;
priority_task_t *priority_task_g = NULL;

void wait_ticks(int ticks)
{
    current_tick += ticks;
    usleep(ticks * TICK_TO_US);
}

void log_printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    fflush(stdout);

    if (log_fp != NULL)
    {
        va_start(args, fmt);
        vfprintf(log_fp, fmt, args);
        va_end(args);
        fflush(log_fp);
    }
}