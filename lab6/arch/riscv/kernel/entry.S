#include "defs.h"

.section .text.entry
.align 2
.globl _traps 
.global __dummy
.global __switch_to

_traps:
    # YOUR CODE HERE
    # -----------
    csrr t0, sscratch
    beq t0, x0, begin_at_s_mode_stack
    csrw sscratch, sp
    mv sp, t0
begin_at_s_mode_stack:

    # 1. save 32 registers and sepc to stack

    addi sp, sp, -8*37

    csrr t1, scause	
	sd t1, 36*8(sp)
	csrr t1, sscratch	
	sd t1, 35*8(sp)
    csrr t1, stval	
	sd t1, 34*8(sp)
	csrr t1, sstatus	
	sd t1, 33*8(sp)
	csrr t1, sepc	
	sd t1, 32*8(sp)
    
	sd x31, 31*8(sp)
	sd x30, 30*8(sp)	
	sd x29, 29*8(sp)	
	sd x28, 28*8(sp)	
	sd x27, 27*8(sp)	
	sd x26, 26*8(sp)	
	sd x25, 25*8(sp)	
	sd x24, 24*8(sp)	
	sd x23, 23*8(sp)	
	sd x22, 22*8(sp)	
	sd x21, 21*8(sp)	
	sd x20, 20*8(sp)	
	sd x19, 19*8(sp)	
	sd x18, 18*8(sp)
	sd x17, 17*8(sp)	
	sd x16, 16*8(sp)	
	sd x15, 15*8(sp)	
	sd x14, 14*8(sp)	
	sd x13, 13*8(sp)	
	sd x12, 12*8(sp)
	sd x11, 11*8(sp)	
	sd x10, 10*8(sp)	
	sd x9, 9*8(sp)	
	sd x8, 8*8(sp)	
	sd x7, 7*8(sp)	
	sd x6, 6*8(sp)	
	sd x5, 5*8(sp)	
	sd x4, 4*8(sp)	
	sd x3, 3*8(sp)	
	sd x2, 2*8(sp)	
	sd x1, 1*8(sp)
	sd x0, 0*8(sp)



	# 2. call trap_handler
	csrr a0,scause
	csrr a1,sepc
    mv a2, sp
	call trap_handler

    .global __ret_from_fork
__ret_from_fork:
    # 利用 sp 从栈中恢复出寄存器的值 
	# 3. restore sepc and 32 registers from stack (x2 (sp) should be restore last)
	

	ld x0, 0*8(sp)
	ld x1, 1*8(sp)
	ld x3, 3*8(sp)
	ld x4, 4*8(sp)
	ld x5, 5*8(sp)
	ld x6, 6*8(sp)
	ld x7, 7*8(sp)
	ld x8, 8*8(sp)
	ld x9, 9*8(sp)
	ld x10, 10*8(sp)
	ld x11, 11*8(sp)
	ld x12, 12*8(sp)
	ld x13, 13*8(sp)
	ld x14, 14*8(sp)
	ld x15, 15*8(sp)
	ld x16, 16*8(sp)
	ld x17, 17*8(sp)
	ld x18, 18*8(sp)
	ld x19, 19*8(sp)
	ld x20, 20*8(sp)
	ld x21, 21*8(sp)
	ld x22, 22*8(sp)
	ld x23, 23*8(sp)
	ld x24, 24*8(sp)
	ld x25, 25*8(sp)
	ld x26, 26*8(sp)
	ld x27, 27*8(sp)
	ld x28, 28*8(sp)
	ld x29, 29*8(sp)
	ld x30, 30*8(sp)
	ld x31, 31*8(sp)

	ld t1, 32*8(sp)
	csrw sepc, t1
    ld t1, 33*8(sp)
    csrw sstatus, t1
    ld t1, 34*8(sp)
	csrw stval, t1
    ld t1, 35*8(sp)
    csrw sscratch, t1
    ld t1, 36*8(sp)
    csrw scause, t1

	ld x2, 2*8(sp)
	
	addi sp, sp, 8*37


    # -----------

    csrr t0, sscratch
    beq t0, x0, end_at_s_mode_stack
    csrw sscratch, sp
    mv sp, t0
end_at_s_mode_stack:
    # 4. return from trap
    sret

    # -----------

__dummy:
    csrr t0, sscratch
    //beq t0, x0, __dummy_begin_at_s_mode_stack
    csrw sscratch, sp
    mv sp, t0
    # YOUR CODE HERE
    # la t0, dummy  
    # csrw sepc, t0
    sret

__switch_to:
    # save state to prev process
    # YOUR CODE HERE

    #保存当前线程的 ra, sp, s0~s11 到当前线程的 thread_struct 中
    #知，一个指针 8 位，一个 unit64 也是 8 位，所以 thread 在 task_struct 中偏移量为 5*8=40
    addi t0, a0, 40
    sd ra, 0(t0)
    addi t0, t0, 8
    sd sp, 0(t0)
    addi t0, t0, 8
    sd s0, 0(t0)
    addi t0, t0, 8
    sd s1, 0(t0)
    addi t0, t0, 8
    sd s2, 0(t0)
    addi t0, t0, 8
    sd s3, 0(t0)
    addi t0, t0, 8
    sd s4, 0(t0)
    addi t0, t0, 8
    sd s5, 0(t0)
    addi t0, t0, 8
    sd s6, 0(t0)
    addi t0, t0, 8
    sd s7, 0(t0)
    addi t0, t0, 8
    sd s8, 0(t0)
    addi t0, t0, 8
    sd s9, 0(t0)
    addi t0, t0, 8
    sd s10, 0(t0)
    addi t0, t0, 8
    sd s11, 0(t0)

    addi t0, t0, 8
    csrr t1, sepc
    sd t1, 0(t0)
    addi t0, t0, 8
    csrr t1, sstatus
    sd t1, 0(t0)
    addi t0, t0, 8
    csrr t1, sscratch
    sd t1, 0(t0)
    addi t0, t0, 8


    # restore state from next process
    #将下一个线程的 thread_struct 中的相关数据载入到 ra、sp、s0~s11 中
    # YOUR CODE HERE

    addi t0, a1, 40
    ld ra, 0(t0)
    addi t0, t0, 8
    ld sp, 0(t0)
    addi t0, t0, 8
    ld s0, 0(t0)
    addi t0, t0, 8
    ld s1, 0(t0)
    addi t0, t0, 8
    ld s2, 0(t0)
    addi t0, t0, 8
    ld s3, 0(t0)
    addi t0, t0, 8
    ld s4, 0(t0)
    addi t0, t0, 8
    ld s5, 0(t0)
    addi t0, t0, 8
    ld s6, 0(t0)
    addi t0, t0, 8
    ld s7, 0(t0)
    addi t0, t0, 8
    ld s8, 0(t0)
    addi t0, t0, 8
    ld s9, 0(t0)
    addi t0, t0, 8
    ld s10, 0(t0)
    addi t0, t0, 8
    ld s11, 0(t0)
    
    addi t0, t0, 8
    ld t1, 0(t0)
    csrw sepc, t1
    addi t0, t0, 8
    ld t1, 0(t0)
    csrw sstatus, t1
    addi t0, t0, 8
    ld t1, 0(t0)
    csrw sscratch, t1

    addi t0, t0, 8
    ld t0, 0(t0)
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
