#ifndef mmu_guard
#define mmu_guard

//MMU: Memory Management Unit

// virtual_address to physical_address
// 实现虚拟地址到物理地址的映射（翻译）
#include <stdint.h>
uint64_t va2pa(uint64_t vaddr);

#endif