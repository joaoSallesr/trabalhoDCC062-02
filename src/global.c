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

int menu(void)
{
    int option;

    printf("=====================================\n");
    printf("      Simulador de Escalonamento     \n");
    printf("=====================================\n");
    printf("1 - Prioridade\n");
    printf("2 - Loteria\n");
    printf("3 - Ambos\n");
    printf("0 - Sair\n");
    printf("-------------------------------------\n");

    while (1)
    {
        printf("Escolha uma opção: ");

        if (scanf("%d", &option) != 1)
        {
            while (getchar() != '\n')
                ; // clear invalid input
            printf("Opção inválida.\n");
            continue;
        }

        if (option >= 0 && option <= 3)
        {
            printf("\n");
            return option;
        }

        printf("Opção inválida.\n");
    }
}