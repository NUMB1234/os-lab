// arch/riscv/kernel/vm.c
#include "vm.h"
#include "printk.h"

extern char _stext[];
extern char _srodata[];
extern char _sdata[];
// 从 vmlinux.lds.S 中引入作为外部变量，用于内存映射

/* early_pgtbl: 用于 setup_vm 进行 1GB 的 映射。 */
unsigned long  early_pgtbl[512] __attribute__((__aligned__(0x1000)));
// 0x1000 字节对齐

/* swapper_pg_dir: kernel pagetable 根目录， 在 setup_vm_final 进行映射。 */
unsigned long  swapper_pg_dir[512] __attribute__((__aligned__(0x1000)));

void setup_vm(void) {
    /* 
    1. 由于是进行 1GB 的映射 这里不需要使用多级页表 
    2. 将 va 的 64bit 作为如下划分： | high bit | 9 bit | 30 bit |
        high bit 可以忽略
        中间9 bit 作为 early_pgtbl 的 index
        低 30 bit 作为 页内偏移 这里注意到 30 = 9 + 9 + 12， 即我们只使用根页表， 根页表的每个 entry 都对应 1GB 的区域。 
    3. Page Table Entry 的权限 V | R | W | X 位设置为 1
    */
   //printk("1\n");
   memset(early_pgtbl, 0x0, (uint64)PGSIZE);
   uint64 pa = PHY_START;
   uint64 va = PHY_START;
   unsigned int index = (va >> 30) & 0x1ff;    
   early_pgtbl[index] = ((pa >> 30) & 0x3ffffff) << 28 | 0xf;
   //只采用 PPN[2] 存储索引，这样物理地址的后 30 位都用作页内偏移，刚好能映射 1GB 的地址空间
   //printk("%d\n",early_pgtbl[index]);
   va = VM_START;
   index = (va >> 30) & 0x1ff;    
   early_pgtbl[index] = ((pa >> 30) & 0x3ffffff) << 28 | 0xf;
   //printk("%d\n",early_pgtbl[index]);
   //printk("2\n");
   return;
}
// 为什么要等值映射？此时pc还在80...，而内存已经跳到ff...，不等值映射会发生page fault<-cgh

void setup_vm_final(void) {
    //printk("1\n");
    memset(swapper_pg_dir, 0x0, PGSIZE);
    //printk("2\n");
    // No OpenSBI mapping required
    uint64 va = VM_START + OPENSBI_SIZE;
    uint64 pa = PHY_START + OPENSBI_SIZE;

    // mapping kernel text X|-|R|V
    uint64 sz = (uint64)_srodata - (uint64)_stext;
    //printk("3\n");
    create_mapping(swapper_pg_dir, va, pa, sz, 0xb);
    //printk("3\n");
    
    // mapping kernel rodata -|-|R|V
    va += sz;
    pa += sz;
    sz = (uint64)_sdata - (uint64)_srodata;
    create_mapping(swapper_pg_dir, va, pa, sz, 0x3);

    // mapping other memory -|W|R|V
    va += sz;
    pa += sz;
    sz = PHY_SIZE - ((uint64)_sdata - (uint64)_stext);
    // 所有的物理内存总共 128M
    create_mapping(swapper_pg_dir, va, pa, sz, 0x7);

    // set satp with swapper_pg_dir
    unsigned long phy_swapper_pg_dir = (unsigned long)swapper_pg_dir - PA2VA_OFFSET;
    // 得到 swapper_pg_dir 对应的物理地址
    __asm__ volatile (
        "mv t0, %[phy_swapper_pg_dir]\n"
        "srli t0, t0, 12\n"
        "addi t1, x0, 8\n"
        "slli t1, t1, 60\n"
        "add t0, t0, t1\n"
        "csrw satp, t0"
        : :[phy_swapper_pg_dir] "r" (phy_swapper_pg_dir)
        :"memory"
    );

    // flush TLB
    asm volatile("sfence.vma zero, zero");

    // flush icache
    asm volatile("fence.i");
    //printk("3\n");
    return;
}


void create_mapping(unsigned long *pgtbl, unsigned long va, unsigned long pa, unsigned long sz, int perm) {
    /*
    root_pgtbl 为根页表的基地址
    va, pa 为需要映射的虚拟地址、物理地址
    sz 为映射的大小
    perm 为映射的读写权限

    创建多级页表的时候可以使用 kalloc() 来获取一页作为页表目录
    可以使用 V bit 来判断页表项是否存在
    */
    unsigned long* new_pgtbl;

    unsigned int vpn[3];    // VPN，用来索引三级页表
    unsigned int ppn[3];    // 各级页表中 pgtbl_entry 存储的ppn
    unsigned long* base_addr[3];    // 存储各级页表的基地址
    unsigned long pgtbl_entry[3];   // 存储各级页表中 vpn 对应 pgtbl_entry 的值

    int cnt = (sz - 1) / PGSIZE + 1;
    // 映射几次，大于 0 小于 PGSIZE 的部分也要完成一次映射

    //printk("va:%llx perm:%d\n",va,perm);

    for (int i = 0; i < cnt; i++) {
        
        base_addr[2] = (unsigned long)pgtbl;
        // 根页表的物理地址
        vpn[2] = (va >> 30) & 0x1ff;
        pgtbl_entry[2] = base_addr[2][vpn[2]];
        // vpn[2] 用于在根页表中索引，得到 pte
        if (!(pgtbl_entry[2] & 1)) {
            // 判断最低位，即V的值，V = 0，说明下一级页表不存在，新建
            new_pgtbl = (unsigned long*)kalloc();
            // 得到的是 va
            ppn[2] = ((unsigned long)new_pgtbl - PA2VA_OFFSET) >> 12;
            // pa >> 12 -> PPN
            pgtbl_entry[2] = (ppn[2] << 10) | 1;
            // 低 10 位为 flag，flag 置为 1，即V = 1
            base_addr[2][vpn[2]] = pgtbl_entry[2];
        }
        //if (perm==31) printk("%llx\n", pgtbl_entry[2]);
        base_addr[1] = (unsigned long*)(((pgtbl_entry[2] >> 10) << 12) + PA2VA_OFFSET);
        // 由上一级页表项中得到的 PPN 得到下一级页表的物理地址 
        vpn[1] = (va >> 21) & 0x1ff;
        pgtbl_entry[1] = base_addr[1][vpn[1]];
        if (!(pgtbl_entry[1] & 1)) {
            new_pgtbl = (unsigned long*)kalloc();
            ppn[1] = ((unsigned long)new_pgtbl - PA2VA_OFFSET) >> 12;
            pgtbl_entry[1] = (ppn[1] << 10) | 1;
            base_addr[1][vpn[1]] = pgtbl_entry[1];
        }
        //if (perm==31) printk("%llx\n", base_addr[1][vpn[1]]);
        base_addr[0] = (unsigned long*)(((pgtbl_entry[1] >> 10) << 12) + PA2VA_OFFSET);
        vpn[0] = (va >> 12) & 0x1ff;
        ppn[0] = pa >> 12;
        pgtbl_entry[0] = (ppn[0] << 10) | perm;
        // 最后一级要指向实际的物理地址，并设置权限 perm
        base_addr[0][vpn[0]] = pgtbl_entry[0];

        va += PGSIZE;
        pa += PGSIZE;
        // 一次映射 PGSIZE 大小的内存
    }
    
    return;
}

int judge_mapping(unsigned long *pgtbl, unsigned long va) {
    /*
    root_pgtbl 为根页表的基地址
    va, pa 为需要映射的虚拟地址、物理地址
    sz 为映射的大小
    perm 为映射的读写权限

    创建多级页表的时候可以使用 kalloc() 来获取一页作为页表目录
    可以使用 V bit 来判断页表项是否存在
    */

   // printk("fork\nva:%lx",va);
    unsigned long* new_pgtbl;

    unsigned int vpn[3];    // VPN，用来索引三级页表
    unsigned int ppn[3];    // 各级页表中 pgtbl_entry 存储的ppn
    unsigned long* base_addr[3];    // 存储各级页表的基地址
    unsigned long pgtbl_entry[3];   // 存储各级页表中 vpn 对应 pgtbl_entry 的值

    // int cnt = (sz - 1) / PGSIZE + 1;
    // // 映射几次，大于 0 小于 PGSIZE 的部分也要完成一次映射

    // //printk("va:%llx perm:%d\n",va,perm);

    // for (int i = 0; i < cnt; i++) {
        
    base_addr[2] = (unsigned long)pgtbl;
    // 根页表的物理地址
    vpn[2] = (va >> 30) & 0x1ff;
    pgtbl_entry[2] = base_addr[2][vpn[2]];
    // vpn[2] 用于在根页表中索引，得到 pte
    if (!(pgtbl_entry[2] & 1)) return 0;
    //if (perm==31) printk("%llx\n", pgtbl_entry[2]);
    base_addr[1] = (unsigned long*)(((pgtbl_entry[2] >> 10) << 12) + PA2VA_OFFSET);
    // 由上一级页表项中得到的 PPN 得到下一级页表的物理地址 
    vpn[1] = (va >> 21) & 0x1ff;
    pgtbl_entry[1] = base_addr[1][vpn[1]];
    if (!(pgtbl_entry[1] & 1)) return 0;
    //if (perm==31) printk("%llx\n", base_addr[1][vpn[1]]);
    base_addr[0] = (unsigned long*)(((pgtbl_entry[1] >> 10) << 12) + PA2VA_OFFSET);
    vpn[0] = (va >> 12) & 0x1ff;
    pgtbl_entry[0] = base_addr[0][vpn[0]];
    if (!(pgtbl_entry[0] & 1)) return 0;
    
    
    return 1;
}


