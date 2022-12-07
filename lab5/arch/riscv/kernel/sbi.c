#include "types.h"
#include "sbi.h"
#include "printk.h"

struct sbiret sbi_ecall(int ext, int fid, unsigned long arg0,
      unsigned long arg1, unsigned long arg2,
      unsigned long arg3, unsigned long arg4,
      unsigned long arg5) {
    
    struct sbiret ret;

    register uint64 a0 asm ("a0") = (uint64)(arg0);
    register uint64 a1 asm ("a1") = (uint64)(arg1);
    register uint64 a2 asm ("a2") = (uint64)(arg2);
    register uint64 a3 asm ("a3") = (uint64)(arg3);
    register uint64 a4 asm ("a4") = (uint64)(arg4);
    register uint64 a5 asm ("a5") = (uint64)(arg5);
    register uint64 a6 asm ("a6") = (uint64)(fid);
    register uint64 a7 asm ("a7") = (uint64)(ext);

    asm volatile ("ecall"
            : "+r" (a0), "+r" (a1)
            : "r" (a2), "r" (a3), "r" (a4), "r" (a5), "r" (a6), "r" (a7)
            : "memory");
    ret.error = a0;
    ret.value = a1;
   //printk("1");
    return ret;
}

struct sbiret  set_time_ecall() {

    struct sbiret ret;
    long error;
    long value;
    
    asm volatile (
        // "mv a0, x0\n"
	// "mv a1, x0\n"
        "rdtime a0\n"
	"li a1, 10000000\n"
	"add a0, a0, a1\n"
	"mv a1, x0\n"
	"mv a2, x0\n"
	"mv a3, x0\n"
	"mv a4, x0\n"
	"mv a5, x0\n"
        "mv a6, x0\n"
	"mv a7, x0\n"
        "ecall\n"
        "mv %[error], a0\n"
	"mv %[value], a1\n"
        : [error]"=r" (error), [value]"=r" (value)
        ::"memory");
    ret.error = error;
    ret.value = value;
    // printk("1\n");
    return ret;
}


// struct sbiret sbi_ecall(int ext, int fid, uint64 arg0,
// 			            uint64 arg1, uint64 arg2,
// 			            uint64 arg3, uint64 arg4,
// 			            uint64 arg5) 
// {
//     // unimplemented   
// 	long error;
// 	long value;
// 	__asm__ volatile (
//         "mv a7, %[Extension_ID]\n"
//         "mv a6, %[Function_ID]\n"
// 		"mv a0, %[arg0]\n"
// 		"mv a1, %[arg1]\n"
// 		"mv a2, %[arg2]\n"
// 		"mv a3, %[arg3]\n"
// 		"mv a4, %[arg4]\n"
// 		"mv a5, %[arg5]\n"
// 		"ecall\n"
//         "mv %[error], a0\n"
// 		"mv %[value], a1\n"
//         : [error] "=r" (error), [value] "=r" (value)
//         : [Extension_ID] "r" (ext), [Function_ID] "r" (fid),
// 		  [arg0] "r" (arg0),[arg1] "r" (arg1),[arg2] "r" (arg2),[arg3] "r" (arg3),[arg4] "r" (arg4),[arg5] "r" (arg5)
//         : "memory"
//     );
// 	struct sbiret ret_val;
// 	ret_val.error=error;
// 	ret_val.value=value;
//     return ret_val;
// }