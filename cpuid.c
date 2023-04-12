#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// cf.
// https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/ia-32-ia-64-benchmark-code-execution-paper.pdf
static inline uint64_t __tsc_start(void)
{
    uint32_t cycles_high, cycles_low;
    asm volatile(
        "cpuid\n\t"
        "rdtsc\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        : "=r"(cycles_high), "=r"(cycles_low)
        :
        : "%rax", "%rbx", "%rcx", "%rdx");
    return ((uint64_t)(cycles_high) << 32) | (uint64_t)(cycles_low);
}

static inline uint64_t __tsc_end(void)
{
    uint32_t cycles_high, cycles_low;
    asm volatile(
        "rdtscp\n\t"
        "mov %%edx, %0\n\t"
        "mov %%eax, %1\n\t"
        "cpuid\n\t"
        : "=r"(cycles_high), "=r"(cycles_low)
        :
        : "%rax", "%rbx", "%rcx", "%rdx");
    return ((uint64_t)(cycles_high) << 32) | (uint64_t)(cycles_low);
}

static inline void cpuid(uint64_t rax, uint64_t rcx){
    uint64_t rbx, rdx;
    asm volatile( "cpuid\n\t"
        : "=a"(rax), "=b"(rbx), "=c"(rcx), "=d"(rdx)
        : "a"(rax), "c"(rcx)
        : );
}

#define N 10000
uint64_t time[N];
int main(int argc, char* argv[]) {

    if (argc < 3) {
        printf("Usage: %s <rax> <rcx>\n", argv[0]);
        return 0;
    }

    uint64_t rax = atoi(argv[1]);
    uint64_t rcx = atoi(argv[2]);

    int i;
    for (i = 0; i < N; i++) {
        uint64_t start = __tsc_start();
        cpuid(rax, rcx);
        uint64_t end = __tsc_end();
        time[i] = end - start;
    }

    for (i=0; i<N; i++) {
        printf("%lu\n", time[i]);
    }

    return 0;
}
