

#ifndef _YOS_GLOBAL_H_
#define _YOS_GLOBAL_H_

#include "type.h"
#include "idt.h"
#include "gdt.h"
#include "process.h"
/* 外部全局变量*/
extern u32	        disp_pos;	//显示位置，位于liba.asm中

extern u8		gdt_ptr[];	//gdt指针
extern DESCRIPTOR	gdt[];	//gdt表

extern u8		idt_ptr[];	//idt指针
extern GATE		idt[];	//idt表


extern irq_handler	irq_table[];


extern PROCESS		proc_table[];	//进程控制块表
extern PROCESS		*p_proc_ready;
extern char		task_stack[];
extern TSS		tss;
extern u32 		k_reenter;	//中断重入

extern TASK 		task_table[] ;
extern TASK 		user_proc_table[] ;
#endif