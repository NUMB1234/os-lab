#ifndef _TRAP_H
#define _TRAP_H

#include "clock.h"
#include "printk.h"

void trap_handler(unsigned long scause, unsigned long sepc);

#endif