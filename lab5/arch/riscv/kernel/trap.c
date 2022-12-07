// trap.c 
#include "trap.h"
#include "types.h"
#include "proc.h"

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

void trap_handler(unsigned long scause, unsigned long sepc, struct pt_regs *regs) {

    if (scause >> 63) {
    //判断是否为interrupt
        unsigned long EC = scause % (1ul<<63);
        if (EC == 5) {
            //printk("[S] Supervisor Mode Timer Interrupt\n");
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
        // else if (syscall_num == 220) {
        //     validate_all_vmas();
        //     printk("into fork\n");
        //     regs->x[10] = sys_clone(regs);
        //     regs->sepc = regs->sepc + 4;
        // } 
        else {
            printk("[S] Unhandled syscall: %lx", syscall_num);
            while (1);
        }
    } 
    // else if (scause == 0xc) {
    //     printk("[S] Supervisor Page Fault, scause: %lx\n", scause);
    //     //do_instruction_page_fault(regs);
    // } 
    // else if (scause == 0xd || scause == 0xf) {
    //     printk("[S] Supervisor Page Fault, scause: %llx\n", scause);
    //     //do_ls_page_fault(regs);
    // } 
    // else {
    //     printk("[S] Unhandled trap, ");
    //     printk("scause: %lx, ", scause);
    //     //printk("stval: %lx, ", regs->stval);
    //     printk("sepc: %lx\n", regs->sepc);
    //     while (1);
    // }
}