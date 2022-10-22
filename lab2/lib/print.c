#include "print.h"
#include "sbi.h"

void puts(char *s) {
    // unimplemented
    for (int i=0; ;i++) {
        if (s[i]!='\0') sbi_ecall(0x1, 0x0, (int)(s[i]), 0, 0, 0, 0, 0);
        else return;
    }
}

void puti(uint64 x) {
    // unimplemented
    uint64 tmp=x;
    uint64 n=1;
    if (tmp/n) {
        while (1) {
	        n*=10;
	        if (tmp/n==0) {
				n/=10;
				break;
			}
        }
    }
    while (1) {
        uint64 out=x/n;
        sbi_ecall(0x1, 0x0, out%10+'0', 0, 0, 0, 0, 0);
        n/=10;
        if (n==0) break;
    }
    return;
}
