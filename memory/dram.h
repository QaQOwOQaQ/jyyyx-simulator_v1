#ifndef dram_guard
#define dram_guard

#include <stdint.h>

#define MM_LEN 1000

// 虚拟地址的空间比物理地址的空间大得多,而我们要实现虚拟地址到物理地址的映射
// 因此我们绝不能直接把虚拟地址拿过来进行访问，而是要做一些工作
// 对于超出范围的虚拟地址我们做%，这样虚拟地址肯定落在物理地址上
// 当然这只是一种最简单的处理，真实的映射可能并不是这样
uint8_t mm[MM_LEN]; // 内存的存储单元是一个字节

uint64_t read64bits_dram(uint64_t paddr);
void write64bits_dram(uint64_t paddr, uint64_t data);

void print_stack();
void print_register();

#endif