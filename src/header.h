#pragma once

#include <inttypes.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TICK_TO_US 1000000

#define LOTTERY_TASKS 10
#define PRIORITY_TASKS 10
#define QUANTUM_TICK 2

#define TICKET_MAX 99
#define PRIORITY_MAX 10
#define ARRIVAL_MAX 60
#define BURST_MAX 20

typedef enum
{
    NEW,
    READY,
    PREEMPTED,
    FINISHED,
} task_status_t;

typedef struct
{
    uint16_t id;
    uint16_t tickets;
    uint16_t arrival_tick;
    uint16_t finished_tick;
    uint16_t burst_ticks;
    uint16_t remaining_ticks;
    task_status_t status;
} lottery_task_t;

typedef struct
{
    uint16_t id;
    uint16_t priority; // 1 = HIGH :: 10 = LOW
    uint16_t arrival_tick;
    uint16_t finished_tick;
    uint16_t burst_ticks;
    uint16_t remaining_ticks;
    task_status_t status;
} priority_task_t;

void lottery_tasks();
void priority_tasks();

void lottery_log();
void priority_log();

/* helper */
extern lottery_task_t *lottery_task_g;
extern priority_task_t *priority_task_g;

extern uint32_t current_tick;
extern FILE *log_fp;

void wait_ticks(int ticks);
void log_printf(const char *fmt, ...);
