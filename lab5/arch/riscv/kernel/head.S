#include "defs.h"

.extern start_kernel


#用来引用一个在其他模块中定义过的符号名，使得这个符号名所表示的数据或函数能在该模块中被使用

    .section .text.init
    .globl _start
_start:
    la sp, boot_stack_top   
    li t0, PA2VA_OFFSET
    sub sp, sp, t0
    call setup_vm
    #j start_kernel
    call relocate
    #j start_kernel

    # ...

    call mm_init
    #j start_kernel
    call setup_vm_final 
    #j start_kernel
    call task_init
    #j start_kernel
    # ------------------

debug:
    # set stvec = _traps
    la x5, _traps 
    csrw stvec, x5

    # set sie[STIE] = 1
    csrr x5, sie
    ori x5, x5, 32  
    csrw sie, x5

    # set first time interrupt
    # mv a0, x0
	# mv a1, x0
    # rdtime a2
	# li a3, 10000000
	# add a2, a2, a3
	# mv a3, x0
	# mv a4, x0
	# mv a5, x0
	# mv a6, x0
	# mv a7, x0
    #j start_kernel
    #call sbi_ecall
    call set_time_ecall
    #j start_kernel

    # set sstatus[SIE] = 1
    # csrr x5, sstatus
    # ori x5, x5, 2  
    # csrw sstatus, x5

    # ------------------

    #j _traps

    j start_kernel

relocate:
    # set ra = ra + PA2VA_OFFSET
    # set sp = sp + PA2VA_OFFSET (If you have set the sp before)

    #li t0, 0x1f000000
    #slli t0, t0, 8
    li t0, PA2VA_OFFSET
    # PA2VA_OFFSET = 0x1f00000000
    add ra, ra, t0
    add sp, sp, t0

    # set satp with early_pgtbl

    la t0, early_pgtbl
    li t1, PA2VA_OFFSET
    sub t0, t0, t1
    srli t0, t0, 12
    # PA >> 12 == PPN
    # PPN 在 satp 的低 43 位
    addi t1, x0, 8
    # 我们使用的 MODE = 8
    slli t1, t1, 60
    # MODE 是 satp 的 63 - 60 位
    add t0, t0, t1
    csrw satp, t0

    # flush tlb
    sfence.vma zero, zero
    
    # flush icache
    fence.i

    ret

    .section .bss.stack        
    #通过.section伪操作来自定义一个段,每一个段以段名为开始, 以下一个段名或者文件结尾为结束
    .globl boot_stack          
    #使得连接程序能够看到boot_stack
boot_stack:
    .space 4096    # <-- change to your stack size
    #.space size, fill 指令保留size个字节的空间，每个字节的值为fill(不写默认0)

    .globl boot_stack_top
boot_stack_top:
