#include <stdbool.h>

typedef unsigned int proc_id;

typedef enum proc_status {
    RUNNING = 0,
    READY = 1,
    BLOCKED = 2 
} proc_status;

typedef struct process {
    proc_id id;
    proc_status status;
} process;

process create_proc();

void destroy_proc(proc_id id);

bool process_exists(proc_id id);

void stop_proc(proc_id id);

bool proc_is_running(proc_id id);

void run_proc(proc_id id);
