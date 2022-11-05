//arch/riscv/kernel/proc.c
#include "proc.h"

extern void __dummy();
extern void __switch_to(struct task_struct* prev, struct task_struct* next);

struct task_struct* idle;           // idle process
struct task_struct* current;        // 指向当前运行线程的 `task_struct`
struct task_struct* task[NR_TASKS]; // 线程数组, 所有的线程都保存在此

/* 线程初始化 创建 NR_TASKS 个线程 */
void task_init() {
    // 1. 调用 kalloc() 为 idle 分配一个物理页
    idle = (struct task_struct*)kalloc();
    // 2. 设置 state 为 TASK_RUNNING;
    idle->state = TASK_RUNNING;
    // 3. 由于 idle 不参与调度 可以将其 counter / priority 设置为 0
    idle->counter = 0;
    idle->priority = 0;
    // 4. 设置 idle 的 pid 为 0
    idle->pid = 0;
    // 5. 将 current 和 task[0] 指向 idle
    current = idle;
    task[0] = idle;

    /* YOUR CODE HERE */

    // 1. 参考 idle 的设置, 为 task[1] ~ task[NR_TASKS - 1] 进行初始化
    // 2. 其中每个线程的 state 为 TASK_RUNNING, counter 为 0, priority 使用 rand() 来设置, pid 为该线程在线程数组中的下标。
    // 3. 为 task[1] ~ task[NR_TASKS - 1] 设置 `thread_struct` 中的 `ra` 和 `sp`,
    // 4. 其中 `ra` 设置为 __dummy （见 4.3.2）的地址,  `sp` 设置为 该线程申请的物理页的高地址

    /* YOUR CODE HERE */
    for (int i = 1; i <= NR_TASKS - 1; i++) {
        task[i] = (struct task_struct*)kalloc();
        task[i]->state = TASK_RUNNING;
        task[i]->counter = 0;
        task[i]->priority = rand();
        task[i]->pid = i;
        task[i]->thread.ra = (uint64)(__dummy);
        task[i]->thread.sp =  (uint64)(task[i]) + (uint64)(PGSIZE);  //PGSIZE在defs.h中定义
    }
    printk("...proc_init done!\n");
}

/* 在时钟中断处理中被调用 用于判断是否需要进行调度 */
void do_timer() {
    // 1. 如果当前线程是 idle 线程 直接进行调度
    // 2. 如果当前线程不是 idle 对当前线程的运行剩余时间减1 若剩余时间仍然大于0 则直接返回 否则进行调度

    /* YOUR CODE HERE */
    if (current == idle)  schedule();
    else {
        //if (current->counter <0 ) current->counter = 0;
        current->counter--;
        if (current->counter > 0) {
            return;
        }
        else schedule();
    }
}

/* 调度程序 选择出下一个运行的线程 */
void schedule() {
    #ifdef SJF
        SJF_schedule();
    #endif

    #ifdef PRIORITY
        Priority_schedule();
    #endif
}

//短作业优先调度
void SJF_schedule() {
    int not_all_zero = 0;
    int min_index = 1;
    int min = 11;
    for (int i = 1; i<= NR_TASKS - 1; i++) {
        if (task[i]->counter == 0 || task[i]->state != TASK_RUNNING)  continue;
        not_all_zero = 1;
        if (task[i]->counter <= min) {
            min_index = i;
            min = task[i]->counter;
        }
    }
    if (not_all_zero) {
        switch_to(task[min_index]);
    }
    else {
        printk("\n");
        for (int i = 1; i<= NR_TASKS - 1; i++) {
            task[i]->counter = rand();
            printk("SET [PID = %d COUNTER = %d]\n", i, task[i]->counter);
        }
        schedule();
    }
}

//优先级调度
void Priority_schedule() {
    int not_all_zero = 0;
    int max_index = 1;
    int max = 0;
    for (int i = 1; i<= NR_TASKS - 1; i++) {
        if (task[i]->counter == 0 || task[i]->state != TASK_RUNNING)  continue;
        not_all_zero = 1;
        if (task[i]->priority >= max) {
            max_index = i;
            max = task[i]->priority;
        }
    }
    if (not_all_zero) {
        switch_to(task[max_index]);
    }
    else {
        printk("\n");
        for (int i = 1; i<= NR_TASKS - 1; i++) {
            task[i]->counter = rand();
            printk("SET [PID = %d PRIORITY = %d COUNTER = %d]\n", i, task[i]->priority, task[i]->counter);
        }
        schedule();
    }
}

/* 线程切换入口函数*/
void switch_to(struct task_struct* next) {
    if (current != next) {
        #ifdef SJF
            printk("\nswitch to [PID = %d COUNTER = %d]\n", next->pid, next->counter);
        #endif

        #ifdef PRIORITY
            printk("\nswitch to [PID = %d PRIORITY = %d COUNTER = %d]\n", next->pid, next->priority, next->counter);
        #endif

        struct task_struct* prev = current;
        current = next;
        __switch_to(prev, next);
    }
}

/* dummy funciton: 一个循环程序, 循环输出自己的 pid 以及一个自增的局部变量 */
void dummy() {
    uint64 MOD = 1000000007;
    uint64 auto_inc_local_var = 0;
    int last_counter = -1;
    while(1) {
        if (last_counter == -1 || current->counter != last_counter) {
            //if (current->counter == 1) current->counter = 0;
            last_counter = current->counter;
            auto_inc_local_var = (auto_inc_local_var + 1) % MOD;
            //printk("counter = %d. ",current->counter);
            printk("[PID = %d] is running. auto_inc_local_var = %d\n", current->pid, auto_inc_local_var);
        }
    }
}
