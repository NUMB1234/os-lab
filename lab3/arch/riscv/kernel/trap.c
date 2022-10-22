// trap.c 
#include "trap.h"

void trap_handler(unsigned long scause, unsigned long sepc) {
    // 通过 `scause` 判断trap类型
    // 如果是interrupt 判断是否是timer interrupt
    // 如果是timer interrupt 则打印输出相关信息, 并通过 `clock_set_next_event()` 设置下一次时钟中断
    // `clock_set_next_event()` 见 4.5 节
    // 其他interrupt / exception 可以直接忽略

    // YOUR CODE HERE
    if (scause && (1ul<<63) == (1ul<<63)) {
    //判断是否为interrupt
        unsigned long EC = scause % (1ul<<63);
        if (EC == 5) {
            do_timer();
            //printk("[S] Supervisor Mode Timer Interrupt\n");
            //printk("1\n");
            clock_set_next_event();
        }
    }
    return;
}
