// 代码(指令和数据)本质上是存在于磁盘上的，所以磁盘是有必要的
#include <stdlib.h>
#include "../memory/instruction.h"
#include "../cpu/register.h"

inst_t program[15] = {
    // uint64_t add(uint64_t a, uint64_t b);
    { // 1
        push_reg,
        { REG,   0, 0, (uint64_t *)&reg.rbp, NULL }, 
        { EMPTY, 0, 0, NULL,                 NULL },
        "push \%rbp"
    },
    { // 2
        mov_reg_reg,
        { REG,   0,   0,   (uint64_t *)&reg.rsp,   NULL },
        { REG,   0,   0,   (uint64_t *)&reg.rbp,   NULL },
        "mov \%rsp \%rbp"
    },
    { // 3
        mov_reg_mem,
        { REG,        0,     0, (uint64_t *)&reg.rdi, NULL },
        { MM_IMM_REG, -0x18, 0, (uint64_t *)&reg.rbp, NULL },
        "mov \%rdi, -0x18(\%rbp)"
    },
    { // 4
        mov_reg_mem,
        { REG,        0,     0, (uint64_t *)&reg.rsi,   NULL },
        { MM_IMM_REG, -0x20, 0, (uint64_t *)&reg.rbp,   NULL },
        "mov \%rsi, -0x20(\%rbp)"
    },
    { // 5
        mov_mem_reg,
        { MM_IMM_REG, -0x18, 0, (uint64_t *)&reg.rbp, NULL },
        { REG,        0,     0, (uint64_t *)&reg.rdx, NULL },
        "mov -0x18(\%rbp), \%rdx"
    },
    { // 6
        mov_mem_reg,
        { MM_IMM_REG, -0x20, 0, (uint64_t *)&reg.rbp,   NULL },
        { REG,        0,     0, (uint64_t *)&reg.rax,   NULL },
        "mov -0x20(\%rbp), \%rax"
    },
    { // 7
        add_reg_reg,
        {REG,  0,  0,  (uint64_t *)&reg.rdx,  NULL},
        {REG,  0,  0,  (uint64_t *)&reg.rax,  NULL},
        "add \%rdx, \%rax"
    },
    { // 8
        mov_reg_mem,
        { REG,        0,    0,  (uint64_t *)&reg.rax,  NULL },
        { MM_IMM_REG, -0x8, 0,  (uint64_t *)&reg.rbp,  NULL },
        "mov \%rax, -0x8(\%rbp)" // bug here
    },
    { // 9
        mov_mem_reg,
        { MM_IMM_REG, -0x8,  0,   (uint64_t *)&reg.rbp,   NULL },
        { REG,        0,     0,   (uint64_t *)&reg.rax,   NULL },
        "mov -0x8(\%rbp), \%rax"
    },
    { // 10
        pop_reg,
        { REG,    0,  0,  (uint64_t *)&reg.rbp, NULL },
        { EMPTY,  0,  0,  NULL,                 NULL },
        "pop \%rbp"
    },
    { // 11
        ret,
        { EMPTY,  0,  0,  NULL,  NULL },
        { EMPTY,  0,  0,  NULL,  NULL },
        "retq"
    },
    // main entry point
    { // 12，第十二条指令，在数组中的下标为11
        mov_reg_reg,
        { REG,  0,  0,  &reg.rdx,  NULL },
        { REG,  0,  0,  &reg.rsi,  NULL },
        "mov \%rdx \%rsi"
    },
    { // 13
        mov_reg_reg,
        { REG,  0,  0,  (uint64_t *)&reg.rax,  NULL},
        { REG,  0,  0,  (uint64_t *)&reg.rdi,  NULL},
        "mov \%rax \%rdi"
    },
    { // 14
        call,
        { IMM,      (uint64_t)&(program[0]),  0,  NULL,  NULL },
        { EMPTY,    0,                        0,  NULL,  NULL },
        "callq <add>"
    },
    { // 15
        mov_reg_mem,
        { REG,          0,    0,  (uint64_t *)&reg.rax,  NULL },
        { MM_IMM_REG,  -0x8,  0,  (uint64_t *)&reg.rbp,  NULL },
        "mov \%rax, -0x8(\%rbp)"
    }
};
