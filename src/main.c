#include "header.h"

FILE *log_fp = NULL;

lottery_task_t *lottery_task_g = NULL;
priority_task_t *priority_task_g = NULL;

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

int main(void)
{
    srand(1234);

    lottery_task_g = malloc(LOTTERY_TASKS * sizeof(lottery_task_t));
    priority_task_g = malloc(PRIORITY_TASKS * sizeof(priority_task_t));

    priority_tasks();

    priority_log();

    return 0;
}