#include "printk.h"
#include "sbi.h"

#include "defs.h"

#include "clock.h"
#include "trap.h"
#include "proc.h"

extern void schedule();
extern void test();

extern char _stext[];
extern char _srodata[];
extern char uapp_start[];

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
    // printk("%I64u\n",csr_read(satp));
    // printk("\nsstatus:");
    // printk("%I64u",csr_read(sstatus));
    // printk("\n");


    // printk("%d", 2022);
    // printk("Hello RISC-V\n");
    // printk("idle process is running!\n");

    // printk("uapp_start = %ld\n", *uapp_start);
    // printk("_srodata = %ld\n", *_srodata);

    // *uapp_start = 1;
    // *_srodata = 0;

    // printk("uapp_start = %ld\n", *uapp_start);
    // printk("_srodata = %ld\n", *_srodata);

    printk("[S-MODE] Hello RISC-V\n");

    schedule();

    test(); // DO NOT DELETE !!!

	return 0;
}
