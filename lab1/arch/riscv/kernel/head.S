.extern start_kernel    
#用来引用一个在其他模块中定义过的符号名，使得这个符号名所表示的数据或函数能在该模块中被使用

    .section .text.entry
    .globl _start
_start:
    # ------------------
    # - your code here -
    # ------------------
    la sp,boot_stack_top     
    beq x0,x0,start_kernel

    .section .bss.stack        
    #通过.section伪操作来自定义一个段,每一个段以段名为开始, 以下一个段名或者文件结尾为结束
    .globl boot_stack          
    #使得连接程序能够看到boot_stack
boot_stack:
    .space 4096    # <-- change to your stack size
    #.space size, fill 指令保留size个字节的空间，每个字节的值为fill(不写默认0)

    .globl boot_stack_top
boot_stack_top:
