    .section .text.entry
    .align 2
    .globl _traps 
_traps:
    # YOUR CODE HERE
    # -----------

    # 1. save 32 registers and sepc to stack
    addi sp, sp, -8
    sd x0, 0(sp)
    addi sp, sp, -8
    sd x1, 0(sp)
    addi sp, sp, -8
    sd x2, 0(sp)
    addi sp, sp, -8
    sd x3, 0(sp)
    addi sp, sp, -8
    sd x4, 0(sp)
    addi sp, sp, -8
    sd x5, 0(sp)
    addi sp, sp, -8
    sd x6, 0(sp)
    addi sp, sp, -8
    sd x7, 0(sp)
    addi sp, sp, -8
    sd x8, 0(sp)
    addi sp, sp, -8
    sd x9, 0(sp)
    addi sp, sp, -8
    sd x10, 0(sp)
    addi sp, sp, -8
    sd x11, 0(sp)
    addi sp, sp, -8
    sd x12, 0(sp)
    addi sp, sp, -8
    sd x13, 0(sp)
    addi sp, sp, -8
    sd x14, 0(sp)
    addi sp, sp, -8
    sd x15, 0(sp)
    addi sp, sp, -8
    sd x16, 0(sp)
    addi sp, sp, -8
    sd x17, 0(sp)
    addi sp, sp, -8
    sd x18, 0(sp)
    addi sp, sp, -8
    sd x19, 0(sp)
    addi sp, sp, -8
    sd x20, 0(sp)
    addi sp, sp, -8
    sd x21, 0(sp)
    addi sp, sp, -8
    sd x22, 0(sp)
    addi sp, sp, -8
    sd x23, 0(sp)
    addi sp, sp, -8
    sd x24, 0(sp)
    addi sp, sp, -8
    sd x25, 0(sp)
    addi sp, sp, -8
    sd x26, 0(sp)
    addi sp, sp, -8
    sd x27, 0(sp)
    addi sp, sp, -8
    sd x28, 0(sp)
    addi sp, sp, -8
    sd x29, 0(sp)
    addi sp, sp, -8
    sd x30, 0(sp)
    addi sp, sp, -8
    sd x31, 0(sp)
    
    csrr x5, sepc
    addi sp, sp, -8
    sd x5, 0(sp)

    # -----------

    # 2. call trap_handler
    csrr x5, scause
    mv a0, x5
    csrr x5, sepc
    mv a1, x5
    call trap_handler   

    # -----------

    # 3. restore sepc and 32 registers (x2(sp) should be restore last) from stack
    ld x5, 0(sp)
    addi sp, sp, 8
    csrw sepc, x5

    ld x31, 0(sp)
    addi sp, sp, 8
    ld x30, 0(sp)
    addi sp, sp, 8
    ld x29, 0(sp)
    addi sp, sp, 8
    ld x28, 0(sp)
    addi sp, sp, 8
    ld x27, 0(sp)
    addi sp, sp, 8
    ld x26, 0(sp)
    addi sp, sp, 8
    ld x25, 0(sp)
    addi sp, sp, 8
    ld x24, 0(sp)
    addi sp, sp, 8
    ld x23, 0(sp)
    addi sp, sp, 8
    ld x22, 0(sp)
    addi sp, sp, 8
    ld x21, 0(sp)
    addi sp, sp, 8
    ld x20, 0(sp)
    addi sp, sp, 8
    ld x19, 0(sp)
    addi sp, sp, 8
    ld x18, 0(sp)
    addi sp, sp, 8
    ld x17, 0(sp)
    addi sp, sp, 8
    ld x16, 0(sp)
    addi sp, sp, 8
    ld x15, 0(sp)
    addi sp, sp, 8
    ld x14, 0(sp)
    addi sp, sp, 8
    ld x13, 0(sp)
    addi sp, sp, 8
    ld x12, 0(sp)
    addi sp, sp, 8
    ld x11, 0(sp)
    addi sp, sp, 8
    ld x10, 0(sp)
    addi sp, sp, 8
    ld x9, 0(sp)
    addi sp, sp, 8
    ld x8, 0(sp)
    addi sp, sp, 8
    ld x7, 0(sp)
    addi sp, sp, 8
    ld x6, 0(sp)
    addi sp, sp, 8
    ld x5, 0(sp)
    addi sp, sp, 8
    ld x4, 0(sp)
    addi sp, sp, 8
    ld x3, 0(sp)
    ld x1, 16(sp)
    ld x0, 24(sp)      
    ld x2, 8(sp)    #最后恢复x2，防止修改当前sp
    addi sp, sp, 24

    # -----------

    # 4. return from trap
    sret

    # -----------
