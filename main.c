#include <stdio.h>
#include "cpu/register.h"
#include "cpu/mmu.h"
#include "memory/instruction.h"
#include "memory/dram.h"
#include "disk/elf.h"

extern inst_t program[];

int main()
{
    puts("-------------begin-------------");
    init_handler_table();

    // init
    reg.rax = 0x12340000;
    reg.rbx = 0x555555555190;
    reg.rcx = 0x555555555190;
    reg.rdx = 0xabcd;
    reg.rsi = 0x7fffffffe4c8;
    reg.rdi = 0x1;
    reg.rbp = 0x7fffffffe3d0;
    reg.rsp = 0x7fffffffe3b0;
    reg.rip = (uint64_t)&program[11]; // program[0]的类型是inst_t，需要装换

    // 绝对不要直接对内存进行读写,这是非常危险的行为，例如：m[va2pa(0x7fffffffe3d0)] = 0x00000000; !!! 危险
    // 通过函数读写
    write64bits_dram(va2pa(0x7fffffffe3b0), 0x0);
    write64bits_dram(va2pa(0x7fffffffe3b8), 0x12340000);
    write64bits_dram(va2pa(0x7fffffffe3c0), 0xabcd);
    write64bits_dram(va2pa(0x7fffffffe3c8), 0x0);
    write64bits_dram(va2pa(0x7fffffffe3d0), 0x0);

    print_register();
    print_stack();

    /*  测试上面的写内存
    uint64_t pa =  va2pa(0x7fffffffe3b8);
    printf("0x%16lx\n", *((uint64_t *)(&mm[pa])));
    */

    /* 测试read函数
    printf("0x%16lx\n", read64bits_dram(va2pa(0x7fffffffe3b8)));
    */

    // run init
    for(int i = 0; i < 15; i ++ )
    {
        instruction_cycle();
        print_register();
        print_stack();
    }

    // 校验
    int match = 1;
    match = match && (reg.rax == 0x1234abcd);
    match = match && (reg.rbx == 0x555555555190);
    match = match && (reg.rcx == 0x555555555190);
    match = match && (reg.rdx == 0x12340000);
    match = match && (reg.rsi == 0xabcd);
    match = match && (reg.rdi == 0x12340000);
    match = match && (reg.rbp == 0x7fffffffe3d0);
    match = match && (reg.rsp == 0x7fffffffe3b0);
    
    if(match == 1)  printf("register matched!\n");
    else printf("register not match!\n");

    match = 1;
    match = match && (read64bits_dram(va2pa(0x7fffffffe3b0)) == 0x0);
    match = match && (read64bits_dram(va2pa(0x7fffffffe3b8)) == 0x12340000);
    match = match && (read64bits_dram(va2pa(0x7fffffffe3c0)) == 0xabcd);
    match = match && (read64bits_dram(va2pa(0x7fffffffe3c8)) == 0x1234abcd);
    match = match && (read64bits_dram(va2pa(0x7fffffffe3d0)) == 0x0);

    if(match == 1)  printf("memory matched!\n");
    else printf("memory not match!\n");

    puts("-------------end-------------");
    return 0;
}
