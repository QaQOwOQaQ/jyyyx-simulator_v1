# makefile 可以粗浅的理解为封装了gcc命令的"bash"脚本
CFLAGS = -Wall -g -O2 -std=gnu99
EXE = program
CODE = cpu/mmu.c memory/dram.c disk/code.c  memory/instruction.c  main.c

.PHONY : program
main: 
	gcc $(CODE) $(CFLAGS) -o $(EXE)
# -I 表示把include目录设置在当前目录下面

run: 
	./$(EXE)
