;
;系统调用
;

;系统调用中断
INT_VECTOR_SYS_CALL     equ     0x90

;系统调用号
NR_get_ticks	        equ     0;
NR_write                equ     1;
NR_get_char              equ     2;
;导出函数
global          get_ticks 
global          write
global          get_char
bits 32
[section .text]

get_ticks:
        mov     eax,NR_get_ticks
        int     INT_VECTOR_SYS_CALL
        ret 

write:
        mov     eax,NR_write
        mov     ebx,[esp + 4]
        mov     ecx,[esp + 8]
        int     INT_VECTOR_SYS_CALL
        ret 

get_char:
        mov     eax,NR_get_char
        int     INT_VECTOR_SYS_CALL
        ret