
#ifndef _YOS_SYSCALL_
#define _YOS_SYSCALL_

#include "type.h"
#include "process.h"
#define NR_SYS_CALL 3



int sys_get_ticks();
void sys_write(PROCESS *p_proc,char *buf,size_t len);
int sys_get_char(PROCESS *p_proc);


#endif