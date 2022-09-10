#include <stdio.h>
#include "cpu/register.h"
#include "memory/instruction.h"
#include "cpu/mmu.h"

extern inst_t program[];

int main()
{
    init_handler_table();

    // init
    reg.rax = 0x12340000;
    reg.rbx = 0x0;
    reg.rcx = 0x800660;
    reg.rdx = 0xabcd;
    reg.rsi = 0x7ffffffee2f8;
    reg.rdi = 0x1;
    reg.rbp = 0x7ffffffee210;
    reg.rsp = 0x7ffffffee1f0;
    reg.rip = (uint64_t)&program[11]; // program[0]的类型是inst_t，需要装换
    
    return 0;
}
