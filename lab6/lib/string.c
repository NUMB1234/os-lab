#include "string.h"

void *memset(void *dst, int c, uint64 n) {
    char *cdst = (char *)dst;
    for (uint64 i = 0; i < n; ++i)
        cdst[i] = c;

    return dst;
}

void *memcpy(void *src, void *dst, uint64 n) {
    char *csrc = (char *)src;
    char *cdst = (char *)dst;
    for (uint64 i = 0; i < n; ++i)
        cdst[i] = csrc[i];

    return dst;
}
