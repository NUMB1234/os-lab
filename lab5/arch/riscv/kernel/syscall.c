#include "syscall.h"

int sys_write(unsigned int fd, char* buf, size_t count) {
    int out_num = 0;
    // if (fd == 1) {
    //     for (int i = 0; i < count && buf[i] != '\0'; i++) {
    //         out_num += printk(buf[i]);
    //     }
    // }
    // return out_num;
    if (fd == 1) {
        buf[count] = '\0';
        out_num = printk(buf);
    }
    return out_num;
}

long sys_getpid() {
    return current->pid;
}