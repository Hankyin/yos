/*
系统调用函数的实现

*/
#include "syscall.h"
#include "global.h"
#include "lib.h"
#include "tty.h"
u32 ticks;
system_call 	sys_call_table[NR_SYS_CALL] = {
                                                sys_get_ticks,
                                                sys_write,
                                                sys_get_char
                                                };
int sys_get_ticks()
{
        print_str("ticks\n",PCOLOR_BLACK);
        return ticks;
}


void sys_write(PROCESS *p_proc,char *buf,size_t len)
{
        
        TTY *tty = &tty_table[p_proc->tty];
        for(int i = 0;i < len;i++)
        {
                //outchar(tty,tty->cur_rel,buf[i]);
                tty->out_buf[i] = buf[i];
                tty->count++;
        }
}

int sys_get_char(PROCESS *p_proc)
{
        
        while(!(tty_table[p_proc->tty].kb_char));
        
        int ch = tty_table[p_proc->tty].kb_char;
        tty_table[p_proc->tty].kb_char = 0;
        return ch;
}