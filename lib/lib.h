/*
内核常用工具函数，函数实现在libc.c与liba.asm中。
*/

#ifndef _YOS_LIB_
#define _YOS_LIB_

#include "type.h"
#include "x86.h"
#include "gdt.h"


/* 显示函数的颜色参数 */
#define PCOLOR_RED      0x7c
#define PCOLOR_BLACK    0x0f

/* 汇编函数 */
void* memcopy(void *dst,void *src,int n);
void print_str(char* str,u8 color);
void out_byte(u16 port,u8 val);
u8 in_byte(u16 port);
void memset(void* p_dst, char ch, int size);
void disable_irq(int irq);
void enable_irq(int irq);
/* C语言实现函数*/
void print_int(int num,u8 color);
char *itoa(char *str,int num);
void delay(u32 sec);
char* strcpy(char* dest, const char* src);
size_t strlen(const char *s);
void init_descriptor(DESCRIPTOR *p_desc,u32 base,u32 limit,u16 attribute);
u32 seg2phys(u16 seg);
void disable_int();
void enable_int();
#endif
