#include "process.h"
#include "lib.h"
#include "global.h"
#include "tty.h"

void task();
void proc();

PROCESS		proc_table[NR_TASKS + NR_PROCS];	//进程控制块表
PROCESS		*p_proc_ready;
char		task_stack[STACK_SIZE_TOTAL];
TASK 		task_table[NR_TASKS] = {{task_tty,STACK_SIZE_TEST,"task_tty"}};
TASK 		user_proc_table[NR_PROCS] = {{proc,STACK_SIZE_TEST,"proc"}};


void schedule()
{
        p_proc_ready++;

	if (p_proc_ready >= proc_table + NR_TASKS + NR_PROCS) {
		p_proc_ready = proc_table;
	}
}

void init_pcb()
{
        print_str("init_pcb start...\n",PCOLOR_BLACK);
        TASK*		p_task		= task_table;
	PROCESS*	p_proc		= proc_table;
	char*		p_task_stack	= task_stack + STACK_SIZE_TOTAL;
	u16		selector_ldt	= SELECTOR_LDT_FIRST;
	int i;
        u8              privilege;
        u8              rpl;
        int             eflags;
	for (i = 0; i < NR_TASKS+NR_PROCS; i++) {
                if (i < NR_TASKS) {     /* 任务 */
                        p_task    = task_table + i;
                        privilege = PRIVILEGE_TASK;
                        rpl       = RPL_TASK;
                        eflags    = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
                }
                else {                  /* 用户进程 */
                        p_task    = user_proc_table + (i - NR_TASKS);
                        privilege = PRIVILEGE_USER;
                        rpl       = RPL_USER;
                        eflags    = 0x202; /* IF=1, bit 2 is always 1 */
                }

		strcpy(p_proc->p_name, p_task->name);	// name of the process
		p_proc->pid = i;			// pid

		p_proc->ldt_sel = selector_ldt;

		memcopy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcopy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
		       sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs	= (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss	= (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs	= (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;

		//p_proc->nr_tty = 0;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}
	p_proc_ready = proc_table;
        print_str("init_pcb end...\n",PCOLOR_BLACK);
}


void task()
{
	while(1)
	{
		print_str("T",PCOLOR_BLACK);
	}
	print_str("T",PCOLOR_RED);
}

void proc()
{
	while(1)
	{
		//print_str("P",PCOLOR_BLACK);
	}
}