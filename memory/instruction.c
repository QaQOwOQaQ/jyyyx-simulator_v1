#include <stdint.h>
#include <stdio.h>
#include "instruction.h"
#include "../cpu/mmu.h"
#include "../cpu/register.h"
#include "../disk/elf.h"

// 实现instrction.h中的decode
uint64_t decode_od(od_t od)
{
    if(od.type == IMM)
    {
        return od.imm;
    }
    else if(od.type == REG)
    {// 寄存器直接寻址，返回寄存器的值
        return (uint64_t)od.reg1;
    }
    else // 内存间接寻址大类
    {
        uint64_t vaddr; // 我们在程序中看到的地址都是虚拟地址，只有当真正需要用的时候才转化为物理地址
        // 这也就是为什么我们需要mmu的原因
        if(od.type == MM_IMM)   
        {
            vaddr = *((uint64_t *)&od.imm);
        }
        else if(od.type == MM_REG)
        {
            vaddr = *od.reg1;
        }
        else if(od.type == MM_REG1_REG2)
        {
            vaddr = *od.reg1 + *od.reg2;
        }
        else if(od.type == MM_IMM_REG1_REG2)
        {
            vaddr = od.imm + *od.reg1 + *od.reg2;
        }
        else if(od.type == MM_REG2_S)
        {
            vaddr = *od.reg2 * od.scal;  
        }
        else if(od.type == MM_IMM_REG2_S)
        {
            vaddr = od.imm + *od.reg2 * od.scal;
        }
        else if(od.type == MM_REG1_REG2_S)
        {
            vaddr = *od.reg1 + *od.reg2 * od.scal;
        }
        else // MM_IMM_REG1_REG2_S
        {
            vaddr = od.imm + *od.reg1 + *od.reg2 * od.scal;
        }

        return va2pa(vaddr);
    }
}

// 实现instrction.h中的 instruction_cycle
void instruction_cycle()
{
    inst_t *instr = (inst_t *)reg.rip; // rip处存放着指令的地址

    uint64_t src = decode_od(instr->src);
    uint64_t dst = decode_od(instr->dst);

    printf("op = 0x%lx\n", (uint64_t)instr->op);
    handler_t handler = handler_table[instr->op];     // sg！！！
    handler(src, dst);
    printf("!!src = 0x%lx \ndst = 0x%lx!!\n", src, dst);
    printf("0x%16lx -- 0x%16lx\n", *(uint64_t *)src, *(uint64_t *)dst);
    printf("    %s\n", instr->code);
}

void init_handler_table()
{ // 初始化函数指针数组
    handler_table[mov_reg_reg] = &mov_reg_reg_handler;
    handler_table[add_reg_reg] = &add_reg_reg_handler;
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

