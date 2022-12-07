#ifndef _VM_H
#define _VM_H

#include "defs.h"
#include "types.h"
#include "string.h"
#include "mm.h"

extern unsigned long swapper_pg_dir[512];

void setup_vm(void);

void setup_vm_final(void);

void create_mapping(uint64 *pgtbl, uint64 va, uint64 pa, uint64 sz, int perm);

#endif