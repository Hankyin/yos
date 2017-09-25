/*
时钟中断处理
*/
#include "lib.h"
#include "process.h"
#include "clock.h"
#include "x86.h"
#include "idt.h"
#include "global.h"
void clock_handler(int irq)
{
	
	//ticks++;

	if (k_reenter != 0) {
		
		return;
	}

    schedule();
}

void init_8253()
{
    out_byte(TIMER_MODE,RATE_GENERATOR);
    out_byte(TIMER0,(u8)(TIMER_FREQ / HZ));
    out_byte(TIMER0,(u8)((TIMER_FREQ / HZ) >> 8));
}

void init_clock()
{
    init_8253();
    	//设置实际中断处理函数
	put_irq_handler(CLOCK_IRQ, clock_handler); /* 设定时钟中断处理程序 */
	enable_irq(CLOCK_IRQ);                     /* 让8259A可以接收时钟中断 */
}