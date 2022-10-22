#include "printk.h"
#include "sbi.h"

#include "defs.h"

#include "clock.h"
#include "trap.h"

extern void test();

int start_kernel() {

    // puts("read:");
    // puti(csr_read(sstatus));    
    // //用csr_cd cdread宏读取sstatus寄存器的值

    // puts("\n");

    // puts("before write:");
    // puti(csr_read(sscratch));
    // //输出sstatus寄存器的值
    // csr_write(sscratch,1);     
    // //用csr_write宏向sscratchs寄存器写入数据
    // puts("\nafter write:");
    // puti(csr_read(sscratch));    
    // //输出sstatus寄存器的值，和之前的比较，用于检验是否写入成功

    // printk("sie:");
    // printk("%I64u",csr_read(sie));
    // printk("\nsstatus:");
    // printk("%I64u",csr_read(sstatus));
    // printk("\n");


    //  printk("%d", 2022);
    //  printk(" Hello RISC-V\n");

    test(); // DO NOT DELETE !!!

	return 0;
}
