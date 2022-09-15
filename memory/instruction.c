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
        else if(od.type == MM_IMM_REG)
        {
            vaddr = od.imm + *(od.reg1);
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

        return va2pa(vaddr); // 在decode中把va转换为pa不是很合适，应该放在命令中做。你知道我想要pa还是va？
        return vaddr;
    }
}

// 实现instrction.h中的 instruction_cycle
void instruction_cycle()
{
    {   // debug block
    // T.T hope no bug here！！！
    // out message: Error!!!!!!!!!!!!!!!!!!!!!!
    // if(count == 6) // zhong duan - -
    // {
    //     puts("Error!!!!!!!!!!!!!!!!!!!!!!");
    //     printf("\n****  %drd instrctions:%s  ****\n\n", count, instr->code);
    //     return ;
    // }
    }
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
    printf("\n****  %drd instrctions:   %s  ****\n\n", count, instr->code);
}

void init_handler_table()
{ // 初始化函数指针数组（注册函数表）
    handler_table[add_reg_reg] = &add_reg_reg_handler;
    handler_table[call]        = &call_handler;
    handler_table[push_reg]    = &push_reg_handler;
    handler_table[pop_reg]     = &pop_reg_handler;
    handler_table[mov_reg_mem] = &mov_reg_mem_handler;
    handler_table[mov_mem_reg] = &mov_mem_reg_handler;
    handler_table[mov_reg_reg] = &mov_reg_reg_handler;
    handler_table[ret]         = &ret_handler;
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

void push_reg_handler(uint64_t src, uint64_t dst)
{
    // src: reg
    // dst: empty

    // push:先申请栈空间再放数据
    reg.rsp = reg.rsp - 8;
    write64bits_dram(va2pa(reg.rsp), *(uint64_t *)src);
    reg.rip = reg.rip + sizeof(inst_t);
}

void pop_reg_handler(uint64_t src, uint64_t dst)
{
    // src: reg
    // dst: empty

    // pop：先取出数据然后释放栈空间
    *(uint64_t *)src = read64bits_dram(va2pa(reg.rsp));
    reg.rsp = reg.rsp + 8;
    reg.rip = reg.rip + sizeof(inst_t);
}

void mov_reg_mem_handler(uint64_t src, uint64_t dst)
{
    // src: reg
    // dst: memory
    write64bits_dram(va2pa(dst), *(uint64_t *)src);
    reg.rip = reg.rip + sizeof(inst_t); 
}

void mov_mem_reg_handler(uint64_t src, uint64_t dst)
{
    // src: mem virtual address
    // dst：reg
    *(uint64_t *)dst = read64bits_dram(va2pa(src));
    reg.rip = reg.rip + sizeof(inst_t);
}

void ret_handler(uint64_t src, uint64_t dst)
{
    // 把保存地址pop出来
    uint64_t ret_addr = read64bits_dram(va2pa(reg.rsp));
    reg.rsp = reg.rsp + 8;
    reg.rip = ret_addr;
}

/*  
    统一解释上文中该语句的含义：*(uint64_t *)src
    src是一个地址，通过(uint64_t *)将它解释为一个指针
    然后通过 * 取得改地址的值
*/