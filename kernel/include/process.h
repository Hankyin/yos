
#ifndef _YOS_PROCESS_H_
#define _YOS_PROCESS_H_
#include "type.h"
#include "gdt.h"
/* Number of tasks */
#define NR_TASKS	1
#define NR_PROCS	1

/* stacks of tasks */
#define STACK_SIZE_TEST		0x8000

#define STACK_SIZE_TOTAL	(STACK_SIZE_TEST * (NR_TASKS+NR_PROCS))


typedef struct s_stackframe {
	u32	gs;		/* \                                    */
	u32	fs;		/* |                                    */
	u32	es;		/* |                                    */
	u32	ds;		/* |                                    */
	u32	edi;		/* |                                    */
	u32	esi;		/* | pushed by save()                   */
	u32	ebp;		/* |                                    */
	u32	kernel_esp;	/* <- 'popad' will ignore it            */
	u32	ebx;		/* |                                    */
	u32	edx;		/* |                                    */
	u32	ecx;		/* |                                    */
	u32	eax;		/* /                                    */
	u32	retaddr;	/* return addr for kernel.asm::save()   */
	u32	eip;		/* \                                    */
	u32	cs;		/* |                                    */
	u32	eflags;		/* | pushed by CPU during interrupt     */
	u32	esp;		/* |                                    */
	u32	ss;		/* /                                    */
}STACK_FRAME;


typedef struct s_porcess {
	STACK_FRAME regs;          /* process registers saved in stack frame */

	u16 ldt_sel;               /* gdt selector giving ldt base and limit */
	DESCRIPTOR ldts[LDT_SIZE]; /* local descriptors for code and data */
	u32 pid;                   /* process id passed in from MM */
	char p_name[16];           /* name of the process */
	u32 tty;			/*启动该进程的tty*/
}PROCESS;


typedef struct s_task {
    task_f initial_eip;//进程的入口函数指针
    int stacksize;//进程堆栈大小
    char name[32];//进程名
    u32 tty;//启动进程的tty
}TASK;


/* 进程调度函数 */
void schedule();

void init_pcb();
#endif