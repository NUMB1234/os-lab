#include "printk.h"
#include "sbi.h"

#include "defs.h"

#include "clock.h"
#include "trap.h"

// Please do not modify

void test() {
    while (1) {
        for (unsigned long i = 0; i < 20000000; i++ ) {}
        printk("kernel is running!\n");
    }
}
