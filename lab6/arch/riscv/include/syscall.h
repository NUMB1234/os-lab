#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "trap.h"
#include "types.h"
#include "proc.h"
#include "vm.h"
#include "mm.h"
#include "defs.h"
#include "string.h"

int sys_write(unsigned int fd, char* buf, size_t count);

long sys_getpid();

long sys_clone(struct pt_regs *regs);

#endif