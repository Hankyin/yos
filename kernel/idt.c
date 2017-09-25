/*
中断初始化相关
*/
#include "idt.h"
#include "lib.h"
#include "global.h"
 u8              idt_ptr[6];	// 0~15:Limit  16~47:Base
 GATE            idt[IDT_SIZE];

 irq_handler     irq_table[NR_IRQ];

/*======================================================================*
                            init_idt
 *======================================================================*/
 void init_idt()
 {
        print_str("init_idt...\n",PCOLOR_BLACK);
 
        init_8259A();
 
         // 全部初始化成中断门(没有陷阱门)
         //异常初始化
         init_idt_desc(INT_VECTOR_DIVIDE,	DA_386IGate,
                       divide_error,		PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_DEBUG,		DA_386IGate,
                       single_step_exception,	PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_NMI,		DA_386IGate,
                       nmi,			PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_BREAKPOINT,	DA_386IGate,
                       breakpoint_exception,	PRIVILEGE_USER);
 
         init_idt_desc(INT_VECTOR_OVERFLOW,	DA_386IGate,
                       overflow,			PRIVILEGE_USER);
 
         init_idt_desc(INT_VECTOR_BOUNDS,	DA_386IGate,
                       bounds_check,		PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_INVAL_OP,	DA_386IGate,
                       inval_opcode,		PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_COPROC_NOT,	DA_386IGate,
                       copr_not_available,	PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_DOUBLE_FAULT,	DA_386IGate,
                       double_fault,		PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_COPROC_SEG,	DA_386IGate,
                       copr_seg_overrun,	PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_INVAL_TSS,	DA_386IGate,
                       inval_tss,		PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_SEG_NOT,	DA_386IGate,
                       segment_not_present,	PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_STACK_FAULT,	DA_386IGate,
                       stack_exception,		PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_PROTECTION,	DA_386IGate,
                       general_protection,	PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_PAGE_FAULT,	DA_386IGate,
                       page_fault,		PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_COPROC_ERR,	DA_386IGate,
                       copr_error,		PRIVILEGE_KRNL);
 
        //外部中断初始化
         init_idt_desc(INT_VECTOR_IRQ0 + 0,      DA_386IGate,
                                 hwint00,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ0 + 1,      DA_386IGate,
                                         hwint01,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ0 + 2,      DA_386IGate,
                                         hwint02,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ0 + 3,      DA_386IGate,
                                         hwint03,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ0 + 4,      DA_386IGate,
                                         hwint04,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ0 + 5,      DA_386IGate,
                                         hwint05,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ0 + 6,      DA_386IGate,
                                         hwint06,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ0 + 7,      DA_386IGate,
                                         hwint07,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ8 + 0,      DA_386IGate,
                                         hwint08,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ8 + 1,      DA_386IGate,
                                         hwint09,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ8 + 2,      DA_386IGate,
                                         hwint10,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ8 + 3,      DA_386IGate,
                                         hwint11,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ8 + 4,      DA_386IGate,
                                         hwint12,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ8 + 5,      DA_386IGate,
                                         hwint13,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ8 + 6,      DA_386IGate,
                                         hwint14,                  PRIVILEGE_KRNL);
 
         init_idt_desc(INT_VECTOR_IRQ8 + 7,      DA_386IGate,
                                         hwint15,                  PRIVILEGE_KRNL);
        //系统调用初始化                             
         init_idt_desc(INT_VECTOR_SYS_CALL,	DA_386IGate,
                                 sys_call,	PRIVILEGE_USER);
             /* idt_ptr[6] 共 6 个字节：0~15:Limit  16~47:Base。用作 sidt/lidt 的参数。*/
         u16* p_idt_limit = (u16*)(&idt_ptr[0]);
         u32* p_idt_base  = (u32*)(&idt_ptr[2]);
         *p_idt_limit = IDT_SIZE * sizeof(GATE) - 1;
         *p_idt_base  = (u32)&idt;
 }
 
 void init_8259A()
 {	
         /*8259初始化*/
         out_byte(INT_M_CTL,	0x11);			// Master 8259, ICW1.
         out_byte(INT_S_CTL,	0x11);			// Slave  8259, ICW1.
         out_byte(INT_M_CTLMASK,	INT_VECTOR_IRQ0);	// Master 8259, ICW2. 设置 '主8259' 的中断入口地址为 0x20.
         out_byte(INT_S_CTLMASK,	INT_VECTOR_IRQ8);	// Slave  8259, ICW2. 设置 '从8259' 的中断入口地址为 0x28
         out_byte(INT_M_CTLMASK,	0x4);			// Master 8259, ICW3. IR2 对应 '从8259'.
         out_byte(INT_S_CTLMASK,	0x2);			// Slave  8259, ICW3. 对应 '主8259' 的 IR2.
         out_byte(INT_M_CTLMASK,	0x1);			// Master 8259, ICW4.
         out_byte(INT_S_CTLMASK,	0x1);			// Slave  8259, ICW4.
 
         /*屏蔽8259a全部中断，每当实现一个中断处理函数后，自行打开对应中断*/
         out_byte(INT_M_CTLMASK,	0xFE);	// Master 8259, OCW1. 
         out_byte(INT_S_CTLMASK,	0xFF);	// Slave  8259, OCW1.
 
         int i;
         for (i = 0; i < NR_IRQ; i++)
                 irq_table[i] = spurious_irq;
 }
 
 /*======================================================================*
                            spurious_irq
                                                    临时8259A中断处理函数
  *======================================================================*/
 void spurious_irq(int irq)
 {
         print_str("spurious_irq: ",PCOLOR_BLACK);
         print_int(irq,PCOLOR_BLACK);
         print_str("\n",PCOLOR_BLACK);
 }
 
 /*======================================================================*
                            put_irq_handler
  *======================================================================*/
 void put_irq_handler(int irq, irq_handler handler)
 {
         disable_irq(irq);
         irq_table[irq] = handler;
 }
 
 /*======================================================================*
                              init_idt_desc
  *----------------------------------------------------------------------*
  初始化 386 中断门
  *======================================================================*/
 void init_idt_desc(unsigned char vector, u8 desc_type,
                           int_handler handler, unsigned char privilege)
 {
         GATE *	p_gate	= &idt[vector];
         u32	base	= (u32)handler;
         p_gate->offset_low	= base & 0xFFFF;
         p_gate->selector	= SELECTOR_KERNEL_CS;
         p_gate->dcount		= 0;
         p_gate->attr		= desc_type | (privilege << 5);
         p_gate->offset_high	= (base >> 16) & 0xFFFF;
 }
 
 /*======================================================================*
                             exception_handler
  *----------------------------------------------------------------------*
  异常处理
  *======================================================================*/
 void exception_handler(int vec_no,int err_code,int eip,int cs,int eflags)
 {
         int i;
         int text_color = 0x74; /* 灰底红字 */
 
         char * err_msg[] = {"#DE Divide Error",
                             "#DB RESERVED",
                             "--  NMI Interrupt",
                             "#BP Breakpoint",
                             "#OF Overflow",
                             "#BR BOUND Range Exceeded",
                             "#UD Invalid Opcode (Undefined Opcode)",
                             "#NM Device Not Available (No Math Coprocessor)",
                             "#DF Double Fault",
                             "    Coprocessor Segment Overrun (reserved)",
                             "#TS Invalid TSS",
                             "#NP Segment Not Present",
                             "#SS Stack-Segment Fault",
                             "#GP General Protection",
                             "#PF Page Fault",
                             "--  (Intel reserved. Do not use.)",
                             "#MF x87 FPU Floating-Point Error (Math Fault)",
                             "#AC Alignment Check",
                             "#MC Machine Check",
                             "#XF SIMD Floating-Point Exception"
         };
 
         /* 通过打印空格的方式清空屏幕的前五行，并把 disp_pos 清零 */
         disp_pos = 0;
         for(i=0;i<80*5;i++){
                 print_str(" ",PCOLOR_BLACK);
         }
         disp_pos = 0;
 
         print_str("Exception! --> ", text_color);
         print_str(err_msg[vec_no], text_color);
         print_str("\n\n", text_color);
         print_str("EFLAGS:", text_color);
         print_int(eflags,PCOLOR_BLACK);
         print_str("\n",PCOLOR_BLACK);
         print_str("CS:", text_color);
         print_int(cs,PCOLOR_BLACK);
         print_str("EIP:", text_color);
         print_int(eip,PCOLOR_BLACK);
         print_int(16,PCOLOR_RED);
         if(err_code != 0xFFFFFFFF){
                 print_str("Error code:", text_color);
                 print_int(err_code,PCOLOR_BLACK);
         }
 }
 
 