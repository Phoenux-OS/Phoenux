#include <kernel.h>
#include <stdint.h>

#define FAILURE -1

task_t** task_table;

void task_init(void) {
    // allocate the task table
    if ((task_table = kalloc(KRNL_MAX_TASKS * sizeof(task_t*))) == 0)
        panic("unable to allocate task table");
    // create kernel task
    if ((task_table[0] = kalloc(sizeof(task_t))) == 0)
        panic("unable to allocate kernel task");
    kstrcpy(task_table[0]->pwd, "/");
    kstrcpy(task_table[0]->name, "kernel");
    task_table[0]->status = KRN_STAT_RES_TASK;    
    return;
}

int current_task = 0;

static int idle_cpu = 1;

void task_spinup(void*);

static const cpu_t default_cpu_status = { 0,0,0,0,0,0,0,0,0,0x1b,0x23,0x23,0x23,0x23,0x23,0x202 };

int task_create(task_t new_task) {
    // find an empty entry in the task table
    int new_pid;
    for (new_pid = 0; new_pid < KRNL_MAX_TASKS; new_pid++)
        if ((!task_table[new_pid]) || (task_table[new_pid] == EMPTY_PID)) break;
    if (new_pid == KRNL_MAX_TASKS)
        return FAILURE;
    
    // allocate a task entry
    if ((task_table[new_pid] = kalloc(sizeof(task_t))) == 0) {
        task_table[new_pid] = EMPTY_PID;
        return FAILURE;
    }

    *task_table[new_pid] = new_task;
    
    return new_pid;
}

extern filesystem_t* filesystems;
int vfs_translate_fs(int mountpoint);

void task_fork(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint32_t ds, uint32_t es, uint32_t fs, uint32_t gs, uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t esp, uint32_t ss) {
    
    // forks the current task in a Unix-like way

    task_table[current_task]->cpu.eax = eax;
    task_table[current_task]->cpu.ebx = ebx;
    task_table[current_task]->cpu.ecx = ecx;
    task_table[current_task]->cpu.edx = edx;
    task_table[current_task]->cpu.esi = esi;
    task_table[current_task]->cpu.edi = edi;
    task_table[current_task]->cpu.ebp = ebp;
    task_table[current_task]->cpu.esp = esp;
    task_table[current_task]->cpu.eip = eip;
    task_table[current_task]->cpu.cs = cs;
    task_table[current_task]->cpu.ds = ds;
    task_table[current_task]->cpu.es = es;
    task_table[current_task]->cpu.fs = fs;
    task_table[current_task]->cpu.gs = gs;
    task_table[current_task]->cpu.ss = ss;
    task_table[current_task]->cpu.eflags = eflags;
    
    task_t new_process = *task_table[current_task];

    new_process.parent = current_task;
    new_process.ipc_queue = 0;
    new_process.ipc_queue_ptr = 0;
    
    *new_process.server_name = 0;
    
    uint32_t task_size = task_table[current_task]->pages * PAGE_SIZE;
    
    // allocate memory for the forked process
    if ((new_process.base = (uint32_t)kalloc(task_size)) == 0) {
        // fail
        task_table[current_task]->cpu.eax = (uint32_t)(FAILURE);
        task_scheduler();
    }
    
    // allocate memory for the file descriptors
    if (!(new_process.file_handles = kalloc(task_table[current_task]->file_handles_ptr * sizeof(file_handle_t)))) {
        // fail
        kfree((void*)new_process.base);
        task_table[current_task]->cpu.eax = (uint32_t)(FAILURE);
        task_scheduler();
    }
    
    // clone the parent's file descriptors
    kmemcpy((char*)new_process.file_handles, (char*)task_table[current_task]->file_handles, task_table[current_task]->file_handles_ptr * sizeof(file_handle_t));
    
    // allocate memory for the new VFS's file descriptors
    if (!(new_process.file_handles_v2 = kalloc(task_table[current_task]->file_handles_v2_ptr * sizeof(file_handle_v2_t)))) {
        // fail
        kfree((void*)new_process.base);
        kfree(new_process.file_handles);
        task_table[current_task]->cpu.eax = (uint32_t)(FAILURE);
        task_scheduler();
    }

    // clone new VFS descriptors
    for (int i = 0; i < task_table[current_task]->file_handles_v2_ptr; i++) {
        file_handle_v2_t new_handle = {0};
        if (task_table[current_task]->file_handles_v2[i].free) {
            new_process.file_handles_v2[i].free = 1;
            continue;
        }
        new_handle.mountpoint = task_table[current_task]->file_handles_v2[i].mountpoint;
        int filesystem = vfs_translate_fs(new_handle.mountpoint);
        new_handle.internal_handle = (*filesystems[filesystem].fork)(task_table[current_task]->file_handles_v2[i].internal_handle);
        new_process.file_handles_v2[i] = new_handle;
    }
    
    // clone the process's memory
    kmemcpy((char*)new_process.base, (char*)task_table[current_task]->base, task_size);
    
    // attempt to create task
    int new_pid = task_create(new_process);
    
    if (new_pid == FAILURE) {
        // fail
        kfree((void*)new_process.base);
        kfree(new_process.file_handles);
        kfree(new_process.file_handles_v2);
        task_table[current_task]->cpu.eax = (uint32_t)(FAILURE);
        task_scheduler();
    }
    
    // return the PID to the forking process
    task_table[current_task]->cpu.eax = (uint32_t)new_pid;
    
    // return 0 in the child process
    task_table[new_pid]->cpu.eax = 0;
    
    task_scheduler();
}

int general_execute_block(task_info_t* task_info) {
    if (general_execute(task_info) == FAILURE)
        return -1;
    task_table[current_task]->status = KRN_STAT_PROCWAIT_TASK;  // start waiting for the child process
    return 0;
}

int general_execute(task_info_t* task_info) {
    // correct the struct pointer for kernel space
    uint32_t task_info_ptr = (uint32_t)task_info;
    task_info_ptr += task_table[current_task]->base;
    task_info = (task_info_t*)task_info_ptr;
    
    // correct pointers for kernel space
    char* path = task_info->path + task_table[current_task]->base;
    char* pwd = task_info->pwd + task_table[current_task]->base;
    char* name = task_info->name + task_table[current_task]->base;
    char* server_name = task_info->server_name + task_table[current_task]->base;
    
    char* ptr_stdin = task_info->stdin + task_table[current_task]->base;
    char* ptr_stdout = task_info->stdout + task_table[current_task]->base;
    char* ptr_stderr = task_info->stderr + task_table[current_task]->base;
    
    vfs_metadata_t metadata;
    
    if (vfs_kget_metadata(path, &metadata, FILE_TYPE) == -2) return FAILURE;

    task_t new_task = {0};
    new_task.status = KRN_STAT_ACTIVE_TASK;
    new_task.cpu = default_cpu_status;
    new_task.parent = current_task;    // set parent
    
    new_task.pages = (TASK_RESERVED_SPACE + metadata.size + DEFAULT_STACK) / PAGE_SIZE;
    if ((TASK_RESERVED_SPACE + metadata.size + DEFAULT_STACK) % PAGE_SIZE) new_task.pages++;
    
    // allocate memory for new process
    if ((new_task.base = (uint32_t)kalloc(new_task.pages * PAGE_SIZE)) == 0)
        return FAILURE;
    
    // load program into memory

    // use the new VFS stack
    int tmp_handle = vfs_kopen(path, O_RDWR, 0);
    vfs_kuread(tmp_handle, (char*)(new_task.base + TASK_RESERVED_SPACE), metadata.size);
    vfs_kclose(tmp_handle);

    /*
    for (uint64_t i = 0; i < metadata.size; i++)
        ((char*)new_task.base)[TASK_RESERVED_SPACE + i] = (char)vfs_kread(path, i);
    */
    
    // attempt to create task
    int new_pid = task_create(new_task);
    
    if (new_pid == FAILURE) {
        // fail
        kfree((void*)new_task.base);
        return FAILURE;
    }
    
    task_table[new_pid]->cpu.esp = ((TASK_RESERVED_SPACE + metadata.size + DEFAULT_STACK) - 1) & 0xfffffff0;
    task_table[new_pid]->cpu.eip = TASK_RESERVED_SPACE;
    
    task_table[new_pid]->heap_base = new_task.pages * PAGE_SIZE;
    task_table[new_pid]->heap_size = 0;
    
    kstrcpy(task_table[new_pid]->pwd, pwd);
    kstrcpy(task_table[new_pid]->name, name);
    kstrcpy(task_table[new_pid]->server_name, server_name);
    
    kstrcpy(task_table[new_pid]->stdin, ptr_stdin);
    kstrcpy(task_table[new_pid]->stdout, ptr_stdout);
    kstrcpy(task_table[new_pid]->stderr, ptr_stderr);
    
    // create file handles 0, 1, and 2
    file_handle_t handle = {0};
    handle.isblock = 1;
    
    kstrcpy(handle.path, ptr_stdin);
    handle.flags = O_RDONLY;
    create_file_handle(new_pid, handle);
    kstrcpy(handle.path, ptr_stdout);
    handle.flags = O_WRONLY;
    create_file_handle(new_pid, handle);
    kstrcpy(handle.path, ptr_stderr);
    handle.flags = O_WRONLY;
    create_file_handle(new_pid, handle);

    // create file handles for std streams
    // this is a huge hack FIXME
    int khandle;
    khandle = vfs_kopen(ptr_stdin, O_RDONLY, 0);
    create_file_handle_v2(new_pid, task_table[0]->file_handles_v2[khandle]);
    task_table[0]->file_handles_v2[khandle].free = 1;
    khandle = vfs_kopen(ptr_stdout, O_WRONLY, 0);
    create_file_handle_v2(new_pid, task_table[0]->file_handles_v2[khandle]);
    task_table[0]->file_handles_v2[khandle].free = 1;
    khandle = vfs_kopen(ptr_stderr, O_WRONLY, 0);
    create_file_handle_v2(new_pid, task_table[0]->file_handles_v2[khandle]);
    task_table[0]->file_handles_v2[khandle].free = 1;

    *((int*)(task_table[new_pid]->base + 0x1000)) = task_info->argc;
    int argv_limit = 0x4000;
    char** argv = (char**)(task_table[new_pid]->base + 0x1010);
    char** src_argv = (char**)((uint32_t)task_info->argv + task_table[current_task]->base);
    // copy the argv's
    for (int i = 0; i < task_info->argc; i++) {
        kstrcpy( (char*)(task_table[new_pid]->base + argv_limit),
                 (char*)(src_argv[i] + task_table[current_task]->base) );
        argv[i] = (char*)argv_limit;
        argv_limit += kstrlen((char*)(src_argv[i] + task_table[current_task]->base)) + 1;
    }
    
    // debug logging
    /*
    kputs("\nNew task startup request completed with:");
    kputs("\npid:         "); kuitoa((uint32_t)new_pid);
    kputs("\nppid:        "); kuitoa((uint32_t)task_table[new_pid]->parent);
    kputs("\nbase:        "); kxtoa(task_table[new_pid]->base);
    kputs("\npages:       "); kxtoa(task_table[new_pid]->pages);
    kputs("\npwd:         "); kputs(task_table[new_pid]->pwd);
    kputs("\nname:        "); kputs(task_table[new_pid]->name);
    kputs("\nserver name: "); kputs(task_table[new_pid]->server_name);
    kputs("\nstdin:       "); kputs(task_table[new_pid]->stdin);
    kputs("\nstdout:      "); kputs(task_table[new_pid]->stdout);
    kputs("\nstderr:      "); kputs(task_table[new_pid]->stderr);
    */

    return new_pid;
}
/*
uint32_t task_start(task_info_t* task_info) {
    // start new task
    // returns 0 on failure, PID on success

    // correct the struct pointer for kernel space
    uint32_t task_info_ptr = (uint32_t)task_info;
    task_info_ptr += task_table[current_task]->base;
    task_info = (task_info_t*)task_info_ptr;
    
    // correct the address for kernel space
    uint32_t task_addr = task_info->addr + task_table[current_task]->base;
    
    // correct pointers for kernel space
    char* pwd = task_info->pwd + task_table[current_task]->base;
    char* name = task_info->name + task_table[current_task]->base;
    char* server_name = task_info->server_name + task_table[current_task]->base;
    
    char* ptr_stdin = task_info->stdin + task_table[current_task]->base;
    char* ptr_stdout = task_info->stdout + task_table[current_task]->base;
    char* ptr_stderr = task_info->stderr + task_table[current_task]->base;
    
    // find an empty entry in the task table
    uint32_t new_task;
    for (new_task = 0; new_task < KRNL_MAX_TASKS; new_task++)
        if (!task_table[new_task]) break;
    if (new_task == KRNL_MAX_TASKS)
        return 0;
    
    // allocate a task entry
    if ((task_table[new_task] = kalloc(sizeof(task_t))) == 0)
        return 0;

    *task_table[new_task] = prototype_task;  // initialise struct
    
    task_table[new_task]->parent = current_task;    // set parent

    // get task size in pages
    task_table[new_task]->pages = (TASK_RESERVED_SPACE + task_info->size + task_info->stack + task_info->heap) / PAGE_SIZE;
    if ((TASK_RESERVED_SPACE + task_info->size + task_info->stack + task_info->heap) % PAGE_SIZE) task_table[new_task]->pages++;

    // allocate task space
    if ((task_table[new_task]->base = (uint32_t)kalloc(task_table[new_task]->pages * PAGE_SIZE)) == 0) {
        kfree(task_table[new_task]);
        task_table[new_task] = 0;
        return 0;
    }
    
    // copy task code into the running location
    kmemcpy((char*)(task_table[new_task]->base + TASK_RESERVED_SPACE), (char*)task_addr, task_info->size);
    
    // build first heap chunk identifier
    task_table[new_task]->heap_begin = (void*)(task_table[new_task]->base + TASK_RESERVED_SPACE + task_info->size + task_info->stack);
    task_table[new_task]->heap_size = task_info->heap;
    heap_chunk_t* heap_chunk = (heap_chunk_t*)task_table[new_task]->heap_begin;
    
    heap_chunk->free = 1;
    heap_chunk->size = task_info->heap - sizeof(heap_chunk_t);
    heap_chunk->prev_chunk = 0;
    
    task_table[new_task]->esp_p = ((TASK_RESERVED_SPACE + task_info->size + task_info->stack) - 1) & 0xfffffff0;
    task_table[new_task]->eip_p = TASK_RESERVED_SPACE;
    
    task_table[new_task]->tty = task_info->tty;
    
    kstrcpy(task_table[new_task]->pwd, pwd);
    kstrcpy(task_table[new_task]->name, name);
    kstrcpy(task_table[new_task]->server_name, server_name);
    
    kstrcpy(task_table[new_task]->stdin, ptr_stdin);
    kstrcpy(task_table[new_task]->stdout, ptr_stdout);
    kstrcpy(task_table[new_task]->stderr, ptr_stderr);
    
    // debug logging
    kputs("\nNew task startup request completed with:");
    kputs("\npid:         "); kuitoa((uint32_t)new_task);
    kputs("\nbase:        "); kxtoa(task_table[new_task]->base);
    kputs("\npages:       "); kxtoa(task_table[new_task]->pages);
    kputs("\ntty:         "); kuitoa((uint32_t)task_table[new_task]->tty);
    kputs("\npwd:         "); kputs(task_table[new_task]->pwd);
    kputs("\nname:        "); kputs(task_table[new_task]->name);
    kputs("\nserver name: "); kputs(task_table[new_task]->server_name);
    kputs("\nstdin:       "); kputs(task_table[new_task]->stdin);
    kputs("\nstdout:      "); kputs(task_table[new_task]->stdout);
    kputs("\nstderr:      "); kputs(task_table[new_task]->stderr);
    
    return new_task;
}
*/
void task_switch(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint32_t ds, uint32_t es, uint32_t fs, uint32_t gs, uint32_t eip, uint32_t cs, uint32_t eflags, uint32_t esp, uint32_t ss) {

    task_table[current_task]->cpu.eax = eax;
    task_table[current_task]->cpu.ebx = ebx;
    task_table[current_task]->cpu.ecx = ecx;
    task_table[current_task]->cpu.edx = edx;
    task_table[current_task]->cpu.esi = esi;
    task_table[current_task]->cpu.edi = edi;
    task_table[current_task]->cpu.ebp = ebp;
    task_table[current_task]->cpu.esp = esp;
    task_table[current_task]->cpu.eip = eip;
    task_table[current_task]->cpu.cs = cs;
    task_table[current_task]->cpu.ds = ds;
    task_table[current_task]->cpu.es = es;
    task_table[current_task]->cpu.fs = fs;
    task_table[current_task]->cpu.gs = gs;
    task_table[current_task]->cpu.ss = ss;
    task_table[current_task]->cpu.eflags = eflags;

    current_task++;
    task_scheduler();
}

extern int read_stat;
extern int write_stat;

void task_scheduler(void) {
    int c;

    for (;;) {
        if (!task_table[current_task]) {
            current_task = 0;
            if (idle_cpu) {
            // if no process took CPU time, wait for the next
            // context switch idling
                ENTER_IDLE;
            }
            idle_cpu = 1;
            continue;
        }
        
        if (task_table[current_task] == EMPTY_PID) {
            current_task++;
            continue;
        }
        
        switch (task_table[current_task]->status) {
            case KRN_STAT_IOWAIT_TASK:
                switch (task_table[current_task]->iowait_type) {
                int done;
                case 0:
                    if ((c = vfs_kread(task_table[current_task]->iowait_dev, task_table[current_task]->iowait_loc)) != IO_NOT_READY) {
                        // embed the result in EAX and continue
                        task_table[current_task]->cpu.eax = (uint32_t)c;
                        task_table[current_task]->status = KRN_STAT_ACTIVE_TASK;
                    } else {
                        current_task++;
                        continue;
                    }
                    break;
                case 1:
                    if ((c = vfs_kwrite(task_table[current_task]->iowait_dev, task_table[current_task]->iowait_loc,
                                        task_table[current_task]->iowait_payload)) != IO_NOT_READY) {
                        // embed the result in EAX and continue
                        task_table[current_task]->cpu.eax = (uint32_t)c;
                        task_table[current_task]->status = KRN_STAT_ACTIVE_TASK;
                    } else {
                        current_task++;
                        continue;
                    }
                    break;
                case 2:
                    done = read(    task_table[current_task]->iowait_handle,
                                    (char*)(task_table[current_task]->iowait_ptr + task_table[current_task]->iowait_done),
                                    task_table[current_task]->iowait_len - task_table[current_task]->iowait_done);
                    if (read_stat) {
                        task_table[current_task]->iowait_done += done;
                        current_task++;
                        continue;
                    } else {
                        task_table[current_task]->cpu.eax = (uint32_t)(task_table[current_task]->iowait_done + done);
                        task_table[current_task]->status = KRN_STAT_ACTIVE_TASK;
                    }
                    break;
                case 3:
                    done = write(    task_table[current_task]->iowait_handle,
                                    (char*)(task_table[current_task]->iowait_ptr + task_table[current_task]->iowait_done),
                                    task_table[current_task]->iowait_len - task_table[current_task]->iowait_done);
                    if (write_stat) {
                        task_table[current_task]->iowait_done += done;
                        current_task++;
                        continue;
                    } else {
                        task_table[current_task]->cpu.eax = (uint32_t)(task_table[current_task]->iowait_done + done);
                        task_table[current_task]->status = KRN_STAT_ACTIVE_TASK;
                    }
                    break;
                default:
                    panic("unrecognised iowait_type");
                }
            case KRN_STAT_ACTIVE_TASK:
                idle_cpu = 0;
                set_segment(0x3, task_table[current_task]->base, task_table[current_task]->pages);
                set_segment(0x4, task_table[current_task]->base, task_table[current_task]->pages);
                task_spinup((void*)task_table[current_task]);
            case KRN_STAT_IPCWAIT_TASK:
            case KRN_STAT_PROCWAIT_TASK:
            case KRN_STAT_VDEVWAIT_TASK:
            case KRN_STAT_RES_TASK:
                current_task++;
                continue;
            default:
                panic("unrecognised task status");
        }

    }

}

extern int ts_enable;

void task_quit_self(int64_t return_value) {
    task_quit(current_task, return_value);
}

void task_quit(int pid, int64_t return_value) {
    int parent = task_table[pid]->parent;
    if (task_table[parent]->status == KRN_STAT_PROCWAIT_TASK) {
        task_table[parent]->cpu.eax = (uint32_t)(return_value & 0xffffffff);
        task_table[parent]->cpu.edx = (uint32_t)((return_value >> 32) & 0xffffffff);
        task_table[parent]->status = KRN_STAT_ACTIVE_TASK;
    }
    kfree((void*)task_table[pid]->file_handles);
    kfree((void*)task_table[pid]->file_handles_v2);
    kfree((void*)task_table[pid]->base);
    kfree((void*)task_table[pid]);
    task_table[pid] = EMPTY_PID;
    DISABLE_INTERRUPTS;
    ts_enable = 1;
    task_scheduler();
}
