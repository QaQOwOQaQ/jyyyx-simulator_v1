#include <stdint.h>
#include <stdio.h>
#include "instruction.h"
#include "../cpu/mmu.h"
#include "../cpu/register.h"
#include "../disk/elf.h"
#include "dram.h"

int count = 0; // inst计数器，放在cycle里面debug那条指令出问题

// 实现instrction.h中的decode
uint64_t decode_od(od_t od)
{
    if(od.type == IMM)
    {
        return *((uint64_t *)&od.imm);
    }
    else if(od.type == REG)
    {// 寄存器直接寻址，返回寄存器的值
        return (uint64_t)od.reg1;
    }
    else if(od.type == EMPTY) // !!!debug: 作者新添加的，不知道它加的啥，我自己加了0，因为0不能访问，正好对应empty！
    {
        return 0;
    }
    else // 内存间接寻址大类
    {
        uint64_t vaddr; // 我们在程序中看到的地址都是虚拟地址，只有当真正需要用的时候才转化为物理地址
        // 这也就是为什么我们需要mmu的原因
        if(od.type == MM_IMM)   
        {
            vaddr = od.imm;
        }
        else if(od.type == MM_REG)
        {
            vaddr = *(od.reg1);
        }
        else if(od.type == MM_REG1_REG2)
        {
            vaddr = *(od.reg1) + *(od.reg2);
        }
        else if(od.type == MM_IMM_REG1_REG2)
        {
            vaddr = od.imm + *(od.reg1) + *(od.reg2);
        }
        else if(od.type == MM_REG2_S)
        {
            vaddr = (*(od.reg2)) * od.scal;  
        }
        else if(od.type == MM_IMM_REG2_S)
        {
            vaddr = od.imm + (*(od.reg2)) * od.scal;
        }
        else if(od.type == MM_REG1_REG2_S)
        {
            vaddr = *(od.reg1) + *(od.reg2) * od.scal;
        }
        else // MM_IMM_REG1_REG2_S
        {
            vaddr = od.imm + *(od.reg1) + (*(od.reg2)) * od.scal;
        }

        return va2pa(vaddr);
    }
}

// 实现instrction.h中的 instruction_cycle
void instruction_cycle()
{
    // debug counter T.T hope no bug here！！！
    ++ count;

    // 取指
    inst_t *instr = (inst_t *)reg.rip; // rip处存放着指令的地址

    // 译码
    uint64_t src = decode_od(instr->src);
    uint64_t dst = decode_od(instr->dst);
    
    // 取函数执行
    handler_t handler = handler_table[instr->op]; 
    handler(src, dst);  

    // 打印指令
    printf("    %s\n", instr->code);
}

void init_handler_table()
{ // 初始化函数指针数组
    handler_table[mov_reg_reg] = &mov_reg_reg_handler;
    handler_table[add_reg_reg] = &add_reg_reg_handler;
    handler_table[call] = &call_handler;
}

void mov_reg_reg_handler(uint64_t src, uint64_t dst)
{
    *(uint64_t *)dst = *(uint64_t *)src;
    reg.rip = reg.rip + sizeof(inst_t);
}

void add_reg_reg_handler(uint64_t src, uint64_t dst)
{
    *(uint64_t *)dst = *(uint64_t *)dst + *(uint64_t *)src;
    reg.rip = reg.rip + sizeof(inst_t);
}

void call_handler(uint64_t src, uint64_t dst)
{
    // src: imm address of called function(被调函数的起始地址)
    // 申请一个空间
    reg.rsp = reg.rsp - 8;
    
    // write return address to rsp memory
    // 保存返回地址--下一条指令的地址
    write64bits_dram(va2pa(reg.rsp), reg.rip + sizeof(inst_t));

    reg.rip = src;
}