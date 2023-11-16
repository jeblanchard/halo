#include "process.h"
#include "stdbool.h"

static proc_id next_proc_id = 1;

proc_id get_new_proc_id() {
    proc_id id_to_ret = next_proc_id;
    next_proc_id++;

    return id_to_ret;
}

process create_proc() {
    return (process) {
        id: get_new_proc_id()
    };
}

#define MAX_PROCS 117
static process alive_procs[MAX_PROCS];

#define PROC_DNE_ID 0

static process DNE_PROC = {id: PROC_DNE_ID,
                                 status: READY};

process* get_proc(proc_id id) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (alive_procs[i].id == id) {
            return &alive_procs[i];
        }
    }

    return &DNE_PROC;
}

void destroy_proc(proc_id id) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (alive_procs[i].id == id) {
            alive_procs[i].id = PROC_DNE_ID;
            return;
        }
    }
}

bool process_exists(proc_id id) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (alive_procs[i].id == id) {
            return true;
        }
    }

    return false;
}

void stop_proc(proc_id id) {
    process* p = get_proc(id);
    p -> status = READY;
}

bool proc_is_running(proc_id id) {
    process* p = get_proc(id);
    if (p -> id == PROC_DNE_ID) {
        return false;
    }

    if (p -> status == RUNNING) {
        return true;
    }

    return false;
}

void run_proc(proc_id id) {
    (void) id;
    
}
