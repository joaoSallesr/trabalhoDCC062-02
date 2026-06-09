#pragma once

#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#define TICK_TO_US 1000000

#define LOTTERY_TASKS 100
#define PRIORITY_TASKS 100
#define QUANTUM_TICK 2

#define TICKET_MAX 100
#define PRIORITY_MAX 10
#define ARRIVAL_MAX 60
#define BURST_MAX 20

extern lottery_task_t *lottery_task_g;
extern priority_task_t *priority_task_g;

typedef enum
{
    NEW,
    READY,
    FINISHED,
} task_status_t;

typedef struct
{
    int id;
    int tickets;
    int arrival_time;
    int burst_time;
    int remaining_time;
    task_status_t status;
} lottery_task_t;

typedef struct
{
    int id;
    int priority; // 1:low / 10:high
    int arrival_time;
    int burst_time;
    int remaining_time;
    task_status_t status;
} priority_task_t;