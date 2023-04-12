// trap.c 
#include "trap.h"
#include "types.h"
#include "proc.h"

extern struct task_struct* current;
extern char ramdisk_start[];
extern pagetable_t pgtbl_task1;

// void trap_handler(unsigned long scause, unsigned long sepc, struct pt_regs *regs) {
//     // 通过 `scause` 判断trap类型
//     // 如果是interrupt 判断是否是timer interrupt
//     // 如果是timer interrupt 则打印输出相关信息, 并通过 `clock_set_next_event()` 设置下一次时钟中断
//     // `clock_set_next_event()` 见 4.5 节
//     // 其他interrupt / exception 可以直接忽略

//     // YOUR CODE HERE
//     if (scause >> 63) {
//     //判断是否为interrupt
//         unsigned long EC = scause % (1ul<<63);
//         if (EC == 5) {
//             //printk("[S] Supervisor Mode Timer Interrupt\n");
//             //printk("1\n");
//             clock_set_next_event();
//             do_timer();
//         }
//     }
//     else if (scause == 8){
//     // uapp 使用 ecall 会产生 ECALL_FROM_U_MODE exception，此时 Interrupt 位为 0 且 Exception Code 为 8
//         syscall(regs);
//     }
//     return;

// }

unsigned long min(unsigned long a, unsigned long b) {
    if (a < b) return a;
    else b;
}

void do_page_fault(struct pt_regs *regs) {
    /*
     1. 通过 stval 获得访问出错的虚拟内存地址（Bad Address）
     2. 通过 find_vma() 查找 Bad Address 是否在某个 vma 中
     3. 分配一个页，将这个页映射到对应的用户地址空间
     4. 通过 (vma->vm_flags | VM_ANONYM) 获得当前的 VMA 是否是匿名空间
     5. 根据 VMA 匿名与否决定将新的页清零或是拷贝 uapp 中的内容
    */

    uint64_t bad_addr = regs->stval;

    struct vm_area_struct* vma = find_vma(current, bad_addr);
    // 当缺页异常发生时，检查 VMA
    if (vma != NULL) {
        // 如果当前访问的虚拟地址在 VMA 中没有记录，认为是不合法的地址，运行出错；反之，进行相应的映射
        uint64_t page = alloc_page();

        if (!(vma->vm_flags & VM_ANONYM)) { 
            // 如果访问的页是存在数据的，如访问的是代码，则需要从文件系统中读取内容，随后进行映射
            // 否则是匿名映射，找一个可用的帧映射上去即可（不用进行任何操作）
            uint64_t fs_addr = (uint64_t)ramdisk_start + vma->vm_content_offset_in_file;
            // printk("start:%lx\addr:%lx\n",vma->vm_start, fs_addr);
            uint64_t page_offset = bad_addr - PGROUNDDOWN(bad_addr);
            memcpy(fs_addr + bad_addr - vma->vm_start, page + page_offset, min(PGSIZE - page_offset, vma->vm_end - bad_addr));
            // 要注意不是从文件 offset 处开始赋值
        }
        
        create_mapping((uint64_t)current->pgd + PA2VA_OFFSET, PGROUNDDOWN(bad_addr), page - PA2VA_OFFSET, PGSIZE, (vma->vm_flags >> 1 << 1) | 0x11);

    }

    return;
}

void trap_handler(unsigned long scause, unsigned long sepc, struct pt_regs *regs) {

    if (scause == 0x8000000000000005) {
    //判断是否为interrupt
        unsigned long EC = scause % (1ul<<63);
        if (EC == 5) {
            printk("[S] Supervisor Mode Timer Interrupt\n");
            // for (int i=0; i<32; i++)
            //     printk("x[%d]:%llx\n",i,regs->x[i]);
            // printk("sepc:%llx\n",regs->sepc);
            // printk("sstatus:%llx\n",regs->sstatus);
            //printk("1\n");
            clock_set_next_event();
            do_timer();
        }
    }
    else if (scause == 8) {
        uint64_t syscall_num = regs->x[17];
        if (syscall_num == 64) {
            regs->x[10] = sys_write(regs->x[10], regs->x[11], regs->x[12]);
            regs->sepc = regs->sepc + 4;
        } 
        else if (syscall_num == 172) {
            regs->x[10] = sys_getpid();
            regs->sepc = regs->sepc + 4;
        }
        else if (syscall_num == 220) {
            regs->x[10] = sys_clone(regs);
            regs->sepc = regs->sepc + 4;
        } 
        else {
            printk("[S] Unhandled syscall: %lx", syscall_num);
            while (1);
        }
    } 
    else if (scause == 12) {
        printk("[S] Supervisor Page Fault, scause: %lx, ", scause);
        printk("stval: %lx, ", regs->stval);
        printk("sepc: %lx\n", regs->sepc);
        do_page_fault(regs);
    } 
    else if (scause == 13 | scause == 15) {
        printk("[S] Supervisor Page Fault, scause: %lx, ", scause);
        printk("stval: %lx, ", regs->stval);
        printk("sepc: %lx\n", regs->sepc);
        do_page_fault(regs);
    } 
    else {
        printk("[S] Unhandled trap, ");
        printk("scause: %lx, ", scause);
        printk("stval: %lx, ", regs->stval);
        printk("sepc: %lx\n", regs->sepc);
        while (1);
    }
}