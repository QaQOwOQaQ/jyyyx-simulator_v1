#ifndef inst_guard
#define inst_guard

#include <stdint.h>
#include <stdlib.h>

typedef enum OP
{   
    mov_reg_reg,        // 0
    mov_reg_mem,        // 1
    mov_mem_reg,        // 2
    push_reg,           // 3
    pop_reg,            // 4
    call,               // 5
    ret,                // 6
    add_reg_reg,        // 7
}op_t;

typedef enum OD_TYPE
{
    EMPTY,
    IMM, REG, 
    MM_IMM, MM_REG, MM_IMM_REG, 
    MM_REG1_REG2, MM_IMM_REG1_REG2, 
    MM_REG2_S, MM_IMM_REG2_S, MM_REG1_REG2_S, 
    MM_IMM_REG1_REG2_S
}od_type_t;

typedef struct OD
{
    od_type_t type;

    int64_t imm;
    int64_t scal;
    /*
        寄存器就相当于C语言的指针
        对寄存器有两种寻址方式：
            1. 寄存器寻址   ：reg
            2. 间接寻址     ：*reg
    */
    uint64_t *reg1;
    uint64_t *reg2;     
}od_t;

typedef struct INSTRUCT_STRUCT
{
    op_t op;  // 操作符 operator
    od_t src; // 源操作数 source operand
    od_t dst; // 目的操作数 destion operand
    char code[100]; // 命令本身 
}inst_t;


// 译码
uint64_t decode_od(od_t od);   
 
// 函数指针
typedef void (*handler_t)(uint64_t, uint64_t);
#define NUM_INSTRTYPE 30                // OP的个数,定义的大一点
handler_t handler_table[NUM_INSTRTYPE]; // 一个函数指针，用来处理不同op时对源操作数和目的操作数的处理，通过下表调用函数避免了大量if-else判断

void init_handler_table();
void instruction_cycle();
void add_reg_reg_handler(uint64_t src, uint64_t dst);
void mov_reg_reg_handler(uint64_t src, uint64_t dst);
void mov_reg_mem_handler(uint64_t src, uint64_t dst);
void mov_mem_reg_handler(uint64_t src, uint64_t dst);
void call_handler       (uint64_t src, uint64_t dst);
void push_reg_handler   (uint64_t src, uint64_t dst);
void pop_reg_handler    (uint64_t src, uint64_t dst);
void ret_handler        (uint64_t src, uint64_t dst);


#endif

