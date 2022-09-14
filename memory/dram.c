#include <stdio.h>
#include "dram.h"
#include "../cpu/mmu.h"
#include "../cpu/register.h"

// 0  --> off
// 1  --> on
#define SRAM_CACHE_SETTING 0

// 为内存设计的一套接口--读写函数
// 往内存里写东西 or 读东西，通过一种间接的方式-函数
// 另外通过读函数和写函数巧妙的将一个64位数写入8个内存单元中（一个内存单元是一个字节）
// 方法是将64位数拆分为8个8位数然后存储在8个内存单元
// 直接访问内存很危险！人总会犯错，并且访问内存很慢的，相较于cached的话！访存是访cache的大约100倍！
uint64_t read64bits_dram(uint64_t paddr)
{
    if(SRAM_CACHE_SETTING == 1)
    {
        return 0x0;
    }

    uint64_t val = 0x0;
    // mm存储的是字节，val是64位数，所以要进行类型转换
    // 否则就出出现很诡异的错误
    val += ((uint64_t)mm[paddr + 0]) << 0;
    val += ((uint64_t)mm[paddr + 1]) << 8;
    val += ((uint64_t)mm[paddr + 2]) << 16;
    val += ((uint64_t)mm[paddr + 3]) << 24;
    val += ((uint64_t)mm[paddr + 4]) << 32;
    val += ((uint64_t)mm[paddr + 5]) << 40;
    val += ((uint64_t)mm[paddr + 6]) << 48;
    val += ((uint64_t)mm[paddr + 7]) << 56;
    return val; 
}

void write64bits_dram(uint64_t paddr, uint64_t data)
{
    if(SRAM_CACHE_SETTING == 1)
    {
        return ;
    }
    mm[paddr + 0] = (data >> 0)  & 0xff; // 掩码，取data低8位
    mm[paddr + 1] = (data >> 8)  & 0xff;
    mm[paddr + 2] = (data >> 16) & 0xff;
    mm[paddr + 3] = (data >> 24) & 0xff;
    mm[paddr + 4] = (data >> 32) & 0xff;
    mm[paddr + 5] = (data >> 40) & 0xff;
    mm[paddr + 6] = (data >> 48) & 0xff;
    mm[paddr + 7] = (data >> 56) & 0xff;
}


// 两个print--debug函数
void print_register() // 打印所有的register--方便查看
{
    printf("rax = 0x%-16lx\nrbx = 0x%-16lx\nrcx = 0x%-16lx\nrdx = 0x%-16lx\n", reg.rax, reg.rbx, reg.rcx, reg.rdx);
    printf("rsi = 0x%-16lx\nrdi = 0x%-16lx\nrbp = 0x%-16lx\nrsp = 0x%-16lx\n", reg.rsi, reg.rdi, reg.rbp, reg.rsp);
    printf("rip = 0x%-16lx\n", reg.rip);
}

void print_stack() // (gbd) x/10x &rsp 
{
    int n = 10;

    uint64_t *high = (uint64_t *)&mm[va2pa(reg.rsp)];
    high = &high[n];
    
    printf("rsp_start: 0x%-16lx\n",reg.rsp);
    uint64_t rsp_start = reg.rsp + n * 8;
    printf("rsp_start: 0x%-16lx\n", rsp_start);

    for(int i = 0; i < 2 * n; i ++ )
    {
        uint64_t *ptr = (uint64_t *)(high - i);
        printf("0x%-16lx : 0x%-16lx", rsp_start, (uint64_t)*ptr);
        if(i == n)
        {
            printf(" <== rsp");
        }

        rsp_start -= 8;

        printf("\n");
    }
}