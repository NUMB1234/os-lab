#ifndef _TRAP_H
#define _TRAP_H

#include "clock.h"
#include "printk.h"

struct pt_regs {
    unsigned long x[32];
    unsigned long sepc;
    unsigned long sstatus;
    unsigned long stval;
    unsigned long sscratch;
    unsigned long scause;
};


unsigned long min(unsigned long a, unsigned long b);

void trap_handler(unsigned long scause, unsigned long sepc, struct pt_regs *regs);

#endif