//arch/riscv/kernel/proc.c
#include "proc.h"
#include "elf.h"

extern void __dummy();
extern void __switch_to(struct task_struct* prev, struct task_struct* next);

extern char ramdisk_start[];
extern char ramdisk_end[];

struct task_struct* idle;           // idle process
struct task_struct* current;        // 指向当前运行线程的 `task_struct`
struct task_struct* task[NR_TASKS]; // 线程数组, 所有的线程都保存在此

extern unsigned long swapper_pg_dir[512];
pagetable_t pgtbl_task1;

/* 线程初始化 创建 NR_TASKS 个线程 */
void task_init() {
    //printk("1");
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
    //printk("1");
    //for (int i = 1; i <= NR_TASKS - 1; i++) {
    int i = 1;
    task[i] = (struct task_struct*)kalloc();
    task[i]->state = TASK_RUNNING;
    task[i]->counter = 0;
    task[i]->priority = rand();
    task[i]->pid = i;
    task[i]->thread.ra = (uint64_t)__dummy;
    task[i]->thread.sp =  (uint64_t)task[i] + PGSIZE;  //PGSIZE在defs.h中定义
    // 对每个用户态进程，其拥有两个 stack：U-Mode Stack 以及 S-Mode Stack
    // 其中 S-Mode Stack 在 lab3 中我们已经设置好了
    // 通过 alloc_page 接口申请一个空的页面来作为 U-Mode Stack 
    uint64_t user_sp = (uint64_t)alloc_page() + PGSIZE; 
    // 为每个用户态进程创建自己的页表，并将 uapp 所在页面以及 U-Mode Stack 做相应的映射
    // 同时为了避免 U-Mode 和 S-Mode 切换的时候切换页表，我们也将内核页表（swapper_pg_dir）复制到每个进程的页表中
    pagetable_t pgtbl = (pagetable_t)alloc_page();
    memcpy(swapper_pg_dir, pgtbl, (uint64)PGSIZE);
    task[i]->pgd = pgtbl;
    load_program(task[i]);
    // uint64_t* uapp_copy = (uint64_t*)alloc_page();  //可能要改alloc的page数
    // uint64_t* uapp_addr = (uint64*)uapp_start;
    // memcpy(uapp_addr, uapp_copy, (uint64)uapp_end - (uint64)uapp_start);
    // uint64 va = USER_START;
    // uint64 pa = (uint64)uapp_copy - PA2VA_OFFSET;
    // uint64 sz = (uint64)uapp_end - (uint64)uapp_start;
    // create_mapping(pgtbl, va, pa, sz, 0x1f);
    // va = USER_END - PGSIZE;
    // pa = user_sp - PGSIZE - PA2VA_OFFSET;
    // sz = PGSIZE;
    // create_mapping(pgtbl, va, pa, sz, 0x17);
    //task[i]->pgd = ((uint64)pgtbl - PA2VA_OFFSET) >> 12 | 0x8000000000000000;
    task[i]->pgd = ((uint64)pgtbl - PA2VA_OFFSET);
    // 对每个用户态进程我们需要将 sepc 修改为 USER_START
    // 配置修改好 sstatus 中的 SPP（使得 sret 返回至 U-Mode）、SPIE（sret 之后开启中断）、SUM（S-Mode 可以访问 User 页面）
    // sscratch 设置为 U-Mode 的 sp，其值为 USER_END（即 U-Mode Stack 被放置在 user space 的最后一个页面）
    // task[i]->thread.sepc = USER_START;
    // uint64_t sstatus = csr_read(sstatus);
    // task[i]->thread.sstatus = sstatus & ~(1 << 8) | 1 << 5 | 1 << 18;
    // task[i]->thread.sscratch = USER_END;
    //}
    printk("...proc_init done!\n");
}


/* 在时钟中断处理中被调用 用于判断是否需要进行调度 */
void do_timer() {
    // 1. 如果当前线程是 idle 线程 直接进行调度
    // 2. 如果当前线程不是 idle 对当前线程的运行剩余时间减1 若剩余时间仍然大于0 则直接返回 否则进行调度

    /* YOUR CODE HERE */
    if (current == idle)  {
        //printk("do_timer:idle\n");
        schedule();
    }
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
        if (task[i]) {
            if (task[i]->counter == 0 || task[i]->state != TASK_RUNNING)  continue;
            not_all_zero = 1;
            if (task[i]->counter <= min) {
                min_index = i;
                min = task[i]->counter;
            }
        }
    }
    if (not_all_zero) {
        //printk("task4\n");
        switch_to(task[min_index]);
    }
    else {
        printk("\n");
        for (int i = 1; i<= NR_TASKS - 1; i++) {
            //task[i]->counter = rand();
            if (task[i]) {
                task[i]->counter = 1;
                printk("SET [PID = %d COUNTER = %d]\n", i, task[i]->counter);
            }
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
        if (task[i]) {
            if (task[i]->counter == 0 || task[i]->state != TASK_RUNNING)  continue;
            not_all_zero = 1;
            if (task[i]->priority >= max) {
                max_index = i;
                max = task[i]->priority;
            }
        }
    }
    if (not_all_zero) {
        switch_to(task[max_index]);
    }
    else {
        //printk("\n");
        for (int i = 1; i<= NR_TASKS - 1; i++) {
            if (task[i]) {
                task[i]->counter = rand();
                printk("SET [PID = %d PRIORITY = %d COUNTER = %d]\n", i, task[i]->priority, task[i]->counter);
            }
        }
        //printk("\n");
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
        //printk("switch_to\n");
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
            //printk("[PID = %d] is running. auto_inc_local_var = %d\n", current->pid, auto_inc_local_var);
            printk("[PID = %d] is running. thread space begin at = 0x%lx\n", current->pid, current);
        }
    }
}

// 解释：https://bbs.pediy.com/thread-274573.htm
/* 将程序 load 进入内存 */
static uint64_t load_program(struct task_struct* task) {
    //printk("load_program");
    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)ramdisk_start;
    // 从地址 uapp_start 开始，便是我们要找的 Ehdr（文件开头）
    //printk("ehdr:%llx\n",ehdr);

    uint64_t phdr_start = (uint64_t)ehdr + ehdr->e_phoff;   
    // e_phoff 字段指明程序头表(program header table)开始处在文件中的偏移量
    // 指向第一个 Phdr
    int phdr_cnt = ehdr->e_phnum;
    // ELF 文件包含的 Segment 的数量

    Elf64_Phdr* phdr;   
    // 是一个 Phdr 数组，其中的每个元素都是一个 Elf64_Phdr
    // 程序头表(program header table)是一个数组， 数组中的每一个元素称为“程序头(program header)”，每一个程序头描述了一个 “段(segment)”
    int load_phdr_cnt = 0;
    uint64 va, pa, sz;
    
    for (int i = 0; i < phdr_cnt; i++) {
        phdr = (Elf64_Phdr*)(phdr_start + sizeof(Elf64_Phdr) * i);
        // 第 i 个 segment
        
        if (phdr->p_type == PT_LOAD) {
        // 说明这个 Segment 的类型是 LOAD，需要在初始化时被加载进内存
            // alloc space and copy content
            // do mapping
            uint64_t memsize = (phdr->p_memsz + (phdr->p_vaddr & 0xfff));
            int num = (memsize - 1) / PGSIZE + 1;
            int perm = ((phdr->p_flags & 1) << 3) | ((phdr->p_flags & 2) << 1) | ((phdr->p_flags & 4) >> 1);
            // VM_READ = 2， VM_WRITE = 4， VM_EXEC = 8，与 p_flags 的 RWX 顺序刚好相反
            do_mmap(task, phdr->p_vaddr, num * PGSIZE, perm, phdr->p_offset, phdr->p_filesz);
            // p_vaddr 表示本段内容的开始位置在进程空间中的虚拟地址
            // p_offset 表明段内容的开始位置相对于文件开头的偏移量（在文件中）
            // 代码和数据区域：该区域从 ELF 给出的 Segment 起始地址 phdr->p_offset 开始，权限参考 phdr->p_flags 进行设置。
            // 将这个页映射到用户空间，供用户程序访问。并设置好对应的 U, W, X, R 权限，最后将 V 置为 1，代表其有效。
            // // uint64_t* phdr_copy = (uint64_t*)alloc_pages((memsize - 1) / PGSIZE + 1);
            // // uint64_t* phdr_addr = (uint64)ehdr + phdr->p_offset;
            // // memset(phdr_copy, 0x0, memsize);
            // // memcpy(phdr_addr, phdr_copy + (phdr->p_vaddr & 0xfff), (uint64)phdr->p_filesz);
            // // 这么些有问题！phdr_copy 是指针 +1 相当于 +8（改版见下）
            // uint64_t* phdr_copy = (uint64_t*)alloc_pages(num);
            // uint64_t* phdr_addr = (uint64_t)ehdr + phdr->p_offset;
            // // p_offset 表明段内容的开始位置相对于文件开头的偏移量（在文件中）
            // memset(phdr_copy, 0x0, memsize);
            // memcpy(phdr_addr, ((uint64_t)phdr_copy) + (phdr->p_vaddr & 0xfff), (uint64)phdr->p_filesz);
            // // (uint64_t)phdr_copy) 对，但是 uint64_t)phdr_copy 错！！该加的括号都加上
            // va = (uint64)phdr->p_vaddr & 0xfffffffffffff000;
            // // p_vaddr 表示本段内容的开始位置在进程空间中的虚拟地址
            // pa = (uint64)phdr_copy - PA2VA_OFFSET;
            // sz = memsize;
            // //printk("p_flags:%d ",phdr->p_flags);
            // //phdr->p_flags = 1;
            // int perm = ((phdr->p_flags & 1) << 3) | ((phdr->p_flags & 2) << 1) | ((phdr->p_flags & 4) >> 1);
            // //printk("perm:%d\n",perm);
            // create_mapping(task->pgd, va, pa, sz, perm | 0x11);

            // load_phdr_cnt ++;
            // //printk("%d\n", load_phdr_cnt);

        }
    }

    // allocate user stack and do mapping
    do_mmap(task, USER_END - PGSIZE, PGSIZE, VM_R_MASK | VM_W_MASK | VM_ANONYM, 0, 0);
    // 用户栈：范围为 [USER_END - PGSIZE, USER_END) ，权限为 VM_READ | VM_WRITE, 并且是匿名的区域
    // uint64_t user_sp = (uint64_t)alloc_page() + PGSIZE; 
    // va = USER_END - PGSIZE;
    // pa = user_sp - PGSIZE - PA2VA_OFFSET;
    // sz = PGSIZE;
    // create_mapping(task->pgd, va, pa, sz, 0x17);


    // following code has been written for you
    // set user stack

    //...;
    // pc for the user program
    task->thread.sepc = ehdr->e_entry;
    // e_entry 是程序的第一条指令被存储的用户态虚拟地址
    // sstatus bits set
    uint64_t sstatus = csr_read(sstatus);
    task->thread.sstatus = sstatus & ~(1 << 8) | 1 << 5 | 1 << 18;
    // user stack for user program
    task->thread.sscratch = USER_END;
    //printk("sepc:%llx\n",task->thread.sepc);
}

/* do_mmap 创建一个新的 vma */
void do_mmap(struct task_struct *task, uint64_t addr, uint64_t length, uint64_t flags, uint64_t vm_content_offset_in_file, uint64_t vm_content_size_in_file) {
    // printk("addr:%lx\noffset:%lx\n",addr, vm_content_offset_in_file);
    task->vmas[task->vma_cnt].vm_start = addr;          
    task->vmas[task->vma_cnt].vm_end = addr + length;           
    task->vmas[task->vma_cnt].vm_flags = flags;          
    task->vmas[task->vma_cnt].vm_content_offset_in_file = vm_content_offset_in_file;              
    task->vmas[task->vma_cnt].vm_content_size_in_file = vm_content_size_in_file;
    task->vma_cnt++;
}

/* find_vma 查找包含某个 addr 的 vma，该函数主要在 Page Fault 处理时起作用 */
struct vm_area_struct *find_vma(struct task_struct *task, uint64_t addr) {
    for (int i = 0; i < task->vma_cnt; i++) {
        struct vm_area_struct vma = task->vmas[i];
        if (vma.vm_start <= addr && addr < vma.vm_end) {
            // 注意区间左开右闭
            return &(task->vmas[i]);
            // 返回一个指针
        }
    }
    return NULL;
}
