
#ifndef _YOS_GDT_H_
#define _YOS_GDT_H_
#include "type.h"

#define GDT_SIZE        128
#define LDT_SIZE	2
#define vir2phys(seg_base,vir) (u32)(((u32)seg_base) + (u32)(vir))
/* 段描述符 */
typedef struct s_descriptor		/* 共 8 个字节 */
{
	u16	limit_low;		/* Limit */
	u16	base_low;		/* Base */
	u8	base_mid;		/* Base */
	u8	attr1;			/* P(1) DPL(2) DT(1) TYPE(4) */
	u8	limit_high_attr2;	/* G(1) D(1) 0(1) AVL(1) LimitHigh(4) */
	u8	base_high;		/* Base */
}DESCRIPTOR;

/* gdtr寄存器结构 */
typedef struct s_gdtr
{
    u32 base;       /*gdt基址*/
    u16 limit;      /*gdt长度*/

}GDTR;

#endif