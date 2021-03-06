
;
P_STACKBASE	equ	0
GSREG		equ	P_STACKBASE
FSREG		equ	GSREG		+ 4
ESREG		equ	FSREG		+ 4
DSREG		equ	ESREG		+ 4
EDIREG		equ	DSREG		+ 4
ESIREG		equ	EDIREG		+ 4
EBPREG		equ	ESIREG		+ 4
KERNELESPREG	equ	EBPREG		+ 4
EBXREG		equ	KERNELESPREG	+ 4
EDXREG		equ	EBXREG		+ 4
ECXREG		equ	EDXREG		+ 4
EAXREG		equ	ECXREG		+ 4
RETADR		equ	EAXREG		+ 4
EIPREG		equ	RETADR		+ 4
CSREG		equ	EIPREG		+ 4
EFLAGSREG	equ	CSREG		+ 4
ESPREG		equ	EFLAGSREG	+ 4
SSREG		equ	ESPREG		+ 4
P_STACKTOP	equ	SSREG		+ 4
P_LDT_SEL	equ	P_STACKTOP
P_LDT		equ	P_LDT_SEL	+ 4

TSS3_S_SP0	equ	4

INT_M_CTL	equ	0x20 ; I/O port for interrupt controller        <Master>
INT_M_CTLMASK	equ	0x21 ; setting bits in this port disables ints  <Master>
INT_S_CTL	equ	0xA0 ; I/O port for second interrupt controller <Slave>
INT_S_CTLMASK	equ	0xA1 ; setting bits in this port disables ints  <Slave>

EOI		equ	0x20

; 以下选择子值必须与 protect.h 中保持一致!!!
SELECTOR_FLAT_C		equ		0x08	; LOADER 里面已经确定了的.
SELECTOR_TSS		equ		0x20	; TSS
SELECTOR_KERNEL_CS	equ		SELECTOR_FLAT_C



;导入函数
extern		init_gdt
extern		init_idt
extern		init_pcb
extern		init_irq

extern 		exception_handler;异常处理函数
extern		spurious_irq;外部中断临时处理函数


;全局变量
extern 		gdt_ptr
extern		idt_ptr
extern		p_proc_ready
extern		tss
extern		disp_pos	;字符显示位置
extern		ticks		;时钟计数值
extern		k_reenter	;中断重入值
extern		irq_table	;中断处理函数表
extern		proc_table	;进程表
extern 		sys_call_table	;系统调用函数表

bits	32

[SECTION .bss]
StackSpace		resb	2 * 1024
StackTop:		; 栈顶,内核堆栈

[section .text]	; 代码在此

;导出函数
global 	_start	; 导出 _start
global	restart	;

;异常处理
global	divide_error
global	single_step_exception
global	nmi
global	breakpoint_exception
global	overflow
global	bounds_check
global	inval_opcode
global	copr_not_available
global	double_fault
global	copr_seg_overrun
global	inval_tss
global	segment_not_present
global	stack_exception
global	general_protection
global	page_fault
global	copr_error
;8259A中断处理
global  hwint00
global  hwint01
global  hwint02
global  hwint03
global  hwint04
global  hwint05
global  hwint06
global  hwint07
global  hwint08
global  hwint09
global  hwint10
global  hwint11
global  hwint12
global  hwint13
global  hwint14
global  hwint15
;系统调用
global	sys_call 


_start:
	; 把 esp 从 LOADER 挪到 KERNEL
	
	mov	esp, StackTop	; 堆栈在 bss 段中
	mov 	dword [disp_pos],2 * 80 * 15
	
	sgdt	[gdt_ptr]
	call	init_gdt
	lgdt	[gdt_ptr] 
	call	init_idt
	lidt	[idt_ptr]
	jmp	SELECTOR_KERNEL_CS:csinit
csinit:		; 这个跳转指令强制使用刚刚初始化的结构
	xor	eax, eax
	mov	ax, SELECTOR_TSS
	ltr	ax
	
	call	init_pcb
	call	init_irq

	mov	dword [k_reenter],0;
	mov 	dword [p_proc_ready],proc_table
	mov 	dword [ticks],0
	jmp	restart
	hlt

sys_call:
        call    save
	
        sti

	push	ecx
	push	ebx
	push	dword [p_proc_ready]
        call    [sys_call_table + eax * 4]
	add	esp, 4 * 3

        mov     [esi + EAXREG - P_STACKBASE], eax
        cli
        ret
; 中断和异常 -- 硬件中断
; ---------------------------------
%macro  hwint_master    1
	call	save

	in 	al,INT_M_CTLMASK	;
	or 	al,(1 << %1)		;禁用当前中断
	out 	INT_M_CTLMASK,al 	;

	mov	al,EOI
	out	INT_M_CTL,al ;发送eoi

	sti 	;CPU在相应中断时会自动关闭中断，想要实现中断重入必须手动打开中断

	push	%1
	call	[irq_table + 4 * %1]	;调用中断处理函数
	add	esp,4

	cli 	;关闭中断

	in 	al,INT_M_CTLMASK	;
	and 	al,~(1 << %1)		;打开当前中断
	out 	INT_M_CTLMASK,al 	;

	ret 
%endmacro
; ---------------------------------

ALIGN   16
hwint00:                ; Interrupt routine for irq 0 (the clock).
	hwint_master	0

ALIGN   16
hwint01:                ; Interrupt routine for irq 1 (keyboard)
        hwint_master    1

ALIGN   16
hwint02:                ; Interrupt routine for irq 2 (cascade!)
        hwint_master    2

ALIGN   16
hwint03:                ; Interrupt routine for irq 3 (second serial)
        hwint_master    3

ALIGN   16
hwint04:                ; Interrupt routine for irq 4 (first serial)
        hwint_master    4

ALIGN   16
hwint05:                ; Interrupt routine for irq 5 (XT winchester)
        hwint_master    5

ALIGN   16
hwint06:                ; Interrupt routine for irq 6 (floppy)
        hwint_master    6

ALIGN   16
hwint07:                ; Interrupt routine for irq 7 (printer)
        hwint_master    7

; ---------------------------------
%macro  hwint_slave     1
        push    %1
        call    spurious_irq
        add     esp, 4
        hlt
%endmacro
; ---------------------------------

ALIGN   16
hwint08:                ; Interrupt routine for irq 8 (realtime clock).
        hwint_slave     8

ALIGN   16
hwint09:                ; Interrupt routine for irq 9 (irq 2 redirected)
        hwint_slave     9

ALIGN   16
hwint10:                ; Interrupt routine for irq 10
        hwint_slave     10

ALIGN   16
hwint11:                ; Interrupt routine for irq 11
        hwint_slave     11

ALIGN   16
hwint12:                ; Interrupt routine for irq 12
        hwint_slave     12

ALIGN   16
hwint13:                ; Interrupt routine for irq 13 (FPU exception)
        hwint_slave     13

ALIGN   16
hwint14:                ; Interrupt routine for irq 14 (AT winchester)
        hwint_slave     14

ALIGN   16
hwint15:                ; Interrupt routine for irq 15
        hwint_slave     15


; 中断和异常 -- 异常
divide_error:
	push	0xFFFFFFFF	; no err code
	push	0		; vector_no	= 0
	jmp	exception
single_step_exception:
	push	0xFFFFFFFF	; no err code
	push	1		; vector_no	= 1
	jmp	exception
nmi:
	push	0xFFFFFFFF	; no err code
	push	2		; vector_no	= 2
	jmp	exception
breakpoint_exception:
	push	0xFFFFFFFF	; no err code
	push	3		; vector_no	= 3
	jmp	exception
overflow:
	push	0xFFFFFFFF	; no err code
	push	4		; vector_no	= 4
	jmp	exception
bounds_check:
	push	0xFFFFFFFF	; no err code
	push	5		; vector_no	= 5
	jmp	exception
inval_opcode:
	push	0xFFFFFFFF	; no err code
	push	6		; vector_no	= 6
	jmp	exception
copr_not_available:
	push	0xFFFFFFFF	; no err code
	push	7		; vector_no	= 7
	jmp	exception
double_fault:
	push	8		; vector_no	= 8
	jmp	exception
copr_seg_overrun:
	push	0xFFFFFFFF	; no err code
	push	9		; vector_no	= 9
	jmp	exception
inval_tss:
	push	10		; vector_no	= A
	jmp	exception
segment_not_present:
	push	11		; vector_no	= B
	jmp	exception
stack_exception:
	push	12		; vector_no	= C
	jmp	exception
general_protection:
	push	13		; vector_no	= D
	jmp	exception
page_fault:
	push	14		; vector_no	= E
	jmp	exception
copr_error:
	push	0xFFFFFFFF	; no err code
	push	16		; vector_no	= 10h
	jmp	exception

exception:
	call	exception_handler
	add	esp, 4*2	; 让栈顶指向 EIP，堆栈中从顶向下依次是：EIP、CS、EFLAGS
	hlt
;================================================================
;中断处理开始，保存现场
;================================================================
save:
	pushad		
	push	ds
	push 	es 
	push	fs 
	push	gs
	mov	dx,ss 
	mov 	ds,dx
	mov 	es,dx 

	mov 	esi,esp	;esi=进程表起始地址

	inc	dword [k_reenter]	;中断重入检测
	cmp	dword [k_reenter], 0	;
	jne	.reenter		;

	mov	esp,StackTop	;切换内核堆栈
	push	restart
	jmp	[esi + RETADR -P_STACKBASE];这句相当于return
.reenter:	
	push 	restart_reenter
	jmp	[esi + RETADR -P_STACKBASE]
;=================================================================
;中断处理完毕，跳转到restart重新启动下一个进程
;=================================================================
restart:
	mov	esp, [p_proc_ready]
	lldt	[esp + P_LDT_SEL] 
	lea	eax, [esp + P_STACKTOP]
	mov	dword [tss + TSS3_S_SP0], eax
restart_reenter:
	dec	dword [k_reenter]
	pop	gs
	pop	fs
	pop	es
	pop	ds
	popad
	add	esp, 4
	iretd
