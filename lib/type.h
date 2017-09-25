/*
 *内核中常用数据类型的定义
 */

#ifndef _YOS_TYPE_
#define _YOS_TYPE_

typedef unsigned int	u32;
typedef unsigned short	u16;
typedef unsigned char	u8;

typedef unsigned int    size_t;

typedef void (*task_f) ();
typedef void (*irq_handler) (int irq);
typedef void (*int_handler) ();

typedef void * system_call;
#endif

