#include "print.h"
#include "sbi.h"

#include "defs.h"

extern void test();

int start_kernel() {

    // puts("read:");
    // puti(csr_read(sstatus));    
    // //用csr_read宏读取sstatus寄存器的值

    // puts("\n");

    // puts("before write:");
    // puti(csr_read(sscratch));
    // //输出sstatus寄存器的值
    // csr_write(sscratch,1);     
    // //用csr_write宏向sscratchs寄存器写入数据
    // puts("\nafter write:");
    // puti(csr_read(sscratch));    
    // //输出sstatus寄存器的值，和之前的比较，用于检验是否写入成功


    puti(2022);
    puts(" Hello RISC-V\n");

    test(); // DO NOT DELETE !!!

	return 0;
}
