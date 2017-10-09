
#ifndef _YOS_TTY_H_
#define _YOS_TTY_H_
#include "type.h"

#define TTY_VMEM_SIZE   80 * 25 * 2  //每个tty所占用的显存
#define TTY_BUF_SIZE    128
#define NR_TTY          7

typedef struct s_tty
{
        u32 tty_num;//当前tty编号
        u8 *out_buf;//输出缓冲
        size_t count;//输出缓冲中有效字符的个数
        u32 kb_char;//键盘输入的字符,每轮循环更新一次
        u8 *addr_start;//tty显存开始地址
        u32 cur_rel;//tty的光标与显存开始地址之间的相对距离
}TTY;   



void task_tty();
void set_current_TTY(TTY *tty);
void set_cursor(u32 pos);
void tty_read_kb(TTY *tty);
void tty_read_wrbuf(TTY *tty);
void tty_write_scr(TTY *tty);
TTY* get_current_TTY();
void outchar(TTY *tty,u32 pos,char ch);


#endif