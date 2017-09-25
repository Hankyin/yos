//内核中重设gdt
#include "lib.h"
#include "type.h"
#include "gdt.h"
#include "global.h"
u8		gdt_ptr[6];	//gdt指针
DESCRIPTOR	gdt[GDT_SIZE];	//gdt表
TSS             tss;
void init_gdt()
{
        print_str("init_gdt start...\n",PCOLOR_BLACK);

	memcopy(&gdt,				  /* New GDT */
	       (void*)(*((u32*)(&gdt_ptr[2]))),   /* Base  of Old GDT */
	       *((u16*)(&gdt_ptr[0])) + 1	  /* Limit of Old GDT */
		);
	/* gdt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base。用作 sgdt/lgdt 的参数。*/
	u16* p_gdt_limit = (u16*)(&gdt_ptr[0]);
	u32* p_gdt_base  = (u32*)(&gdt_ptr[2]);
	*p_gdt_limit = GDT_SIZE * sizeof(DESCRIPTOR) - 1;
	*p_gdt_base  = (u32)&gdt;

	/* 填充 GDT 中 TSS 这个描述符 */
	memset(&tss, 0, sizeof(tss));
	tss.ss0 = SELECTOR_KERNEL_DS;
	init_descriptor(&gdt[INDEX_TSS],
			vir2phys(seg2phys(SELECTOR_KERNEL_DS), &tss),
			sizeof(tss) - 1,
			DA_386TSS);
	tss.iobase = sizeof(tss); /* 没有I/O许可位图 */

	/* 填充 GDT 中进程的 LDT 的描述符 */
	int i;
	PROCESS *proc = proc_table;
	u16 selector_ldt = INDEX_LDT_FIRST << 3;

	for(i = 0;i < NR_TASKS + NR_PROCS;i++)
	{
		init_descriptor(&gdt[selector_ldt >> 3],
				vir2phys(seg2phys(SELECTOR_KERNEL_DS),proc_table[i].ldts),
				LDT_SIZE * sizeof(DESCRIPTOR) -1,
				DA_LDT);
		proc++;
		selector_ldt += 1 << 3;
	}


        print_str("init_gdt end...\n",PCOLOR_BLACK);
}


