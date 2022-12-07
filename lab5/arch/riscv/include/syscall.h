#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "trap.h"
#include "types.h"
#include "proc.h"

int sys_write(unsigned int fd, char* buf, size_t count);

long sys_getpid();

#endif