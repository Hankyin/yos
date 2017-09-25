
#ifndef _YOS_CLOCK_H_
#define _YOS_CLOCK_H_
#include "process.h"


/*实际中断处理函数*/
void clock_handler(int irq);

/*辅助函数*/
void init_8253();//初始化时钟中断控制器
void init_clock();
#endif