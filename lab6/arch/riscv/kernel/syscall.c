#include "syscall.h"

extern void __ret_from_fork();
extern unsigned long swapper_pg_dir[512];

int sys_write(unsigned int fd, char* buf, size_t count) {
    int out_num = 0;
    // if (fd == 1) {
    //     for (int i = 0; i < count && buf[i] != '\0'; i++) {
    //         out_num += printk(buf[i]);
    //     }
    // }
    // return out_num;
    if (fd == 1) {
        buf[count] = '\0';
        out_num = printk(buf);
    }
    return out_num;
}

long sys_getpid() {
    return current->pid;
}

long sys_clone(struct pt_regs *regs) {
/*
1. 参考 task_init 创建一个新的 task, 将的 parent task 的整个页复制到新创建
的 task_struct 页上(这一步复制了哪些东西?）。将 thread.ra 设置
__ret_from_fork, 并正确设置 thread.sp
2. 利用参数 regs 来计算出 child task 的对应的 pt_regs 的地址， 并将其中的 
a0, sp, sepc 设置成正确的值
3. 为 child task 申请 user stack, 并将 parent task 的 user stack 数据复
制到其中
4. 为 child task 分配一个根页表，并仿照 setup_vm_final 完成内核空间的映射
5. 根据 parent task 的页表和 vma 来分配并拷贝 child task 在用户态会用到的
内存
6. 返回子 task 的 pid
*/
    int pid = 0;
    for (int i = 1; i < NR_TASKS; i++) {
        if (!task[i]) {
            pid = i;
            break;
        }
    }
    printk("[S] New task: %d\n", pid);
    // 如果 task[pid] == NULL, 说明这个 pid 还没有被占用，可以作为新 task 的 pid，并将 task[pid] 赋值为新的 struct task_struct*
    task[pid] = (struct task_struct*)kalloc();
    memcpy(current, task[pid], PGSIZE);
    task[pid]->pid = pid;
    task[pid]->thread.ra = (uint64_t)__ret_from_fork;
    struct pt_regs *child_pt_regs = (struct pt_regs*)(task[pid] + ((uint64_t)regs & 0xfff));
    // (uint64_t)regs & 0xfff 得到页内偏移量
    task[pid]->thread.sp = (uint64_t)child_pt_regs; 
	// ra 控制程序的执行流，使得能够跳转到 context switch 的后半段
    // sp 指向 child 的 pt_regs，实现从内核态的栈上恢复出我们在 sys_clone 时拷贝到新的 task 的栈上的、原本在 context switch 时被压入父 task 的寄存器值
    uint64_t user_sp = (uint64_t)alloc_page() + PGSIZE; 
    memcpy(regs->x[2], user_sp - PGSIZE + (regs->x[2] -PGROUNDDOWN(regs->x[2])), PGSIZE - (regs->x[2] -PGROUNDDOWN(regs->x[2])));    
    // 分配用户栈，并拷贝 parent 的栈的内容
    child_pt_regs->x[10] = 0;    // a0
    child_pt_regs->x[2] = user_sp;    // sp
    child_pt_regs->sepc = regs->sepc + 4;    // sepc
    pagetable_t pgtbl = (pagetable_t)alloc_page();
    memcpy(swapper_pg_dir, pgtbl, (uint64)PGSIZE);
    task[pid]->pgd = ((uint64)pgtbl - PA2VA_OFFSET);
    for (int i = 0; i < current->vma_cnt; i++) { 
        for (int va = task[pid]->vmas[i].vm_start; va < task[pid]->vmas[i].vm_end; va += PGSIZE) {
            // printk("fork\n");
            // while (1);
            if (judge_mapping((uint64_t)current->pgd + PA2VA_OFFSET, PGROUNDDOWN(va))) {
                // printk("fork\n");
                uint64_t page = alloc_page();
                uint64_t page_offset = va - PGROUNDDOWN(va);
                memcpy(va, page + page_offset, min(PGSIZE - page_offset, task[pid]->vmas[i].vm_end - va));
                // memcpy(PGROUNDDOWN(va), page, PGSIZE);
            // memcpy(fs_addr + bad_addr - vma->vm_start, page + page_offset, min(PGSIZE - page_offset, vma->vm_end - bad_addr));

                create_mapping((uint64_t)task[pid]->pgd + PA2VA_OFFSET, PGROUNDDOWN(va), page - PA2VA_OFFSET, PGSIZE, (task[pid]->vmas[i].vm_flags >> 1 << 1) | 0x11);
            }
            // while(1);
        }
    }
    
    /*除了内核态之外，你还需要深拷贝一份页表，并遍历页表中映射到 parent task 用户地址空间的页表项
    （为了减小开销，你需要根据 parent task 的 vmas 来 walk page table），这些应该由 parent task 
    专有的页，如果已经分配并且映射到 parent task 的地址空间中了，就需要你另外分配空间，并从原来的内
    存中拷贝数据到新开辟的空间，然后将新开辟的页映射到 child task 的地址空间中。*/
    return pid;
}


// 也就是说，我们通过控制 ra 寄存器，来控制程序的执行流，让它跳转到 context switch 的后半段；通过控制 sp 寄存器，从内核态的栈上恢复出我们在 sys_clone 时拷贝到新的 task 的栈上的，原本在 context switch 时被压入父 task 的寄存器值，然后通过 sret 直接跳回用户态执行用户态程序。