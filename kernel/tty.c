
/*
系统中的任务
*/
#include "tty.h"
#include "keyboard.h"
#include "type.h"
#include "lib.h"
#include "global.h"


TTY tty_table[NR_TTY];//tty表应不应该放到全局变量中呢？
WR_BUF wr_buf[NR_TTY];//sys_write函数会将输出保存在这个缓冲中，然后tty去读取。

int current_TTY = 100;//开始指定一个不可能的tty

void task_tty()
{
        //初始化tty
        for(int i = 0;i < NR_TTY;i++)
        {
                tty_table[i].tty_num = i;     
                tty_table[i].addr_start = (u8 *)(V_MEM_BASE + TTY_VMEM_SIZE * i);
                tty_table[i].cur_rel = 0;
                tty_table[i].count = 0;
                memset(tty_table[i].out_buf,'\0',TTY_BUF_SIZE);
                
                wr_buf[i].is_ready = 0;
                wr_buf[i].is_reading = 0;
                
        }
        
        tty_table[0].cur_rel = disp_pos / 2;
        set_current_TTY(&tty_table[0]);
        
        while(1)
        {
                
                for(int i = 0;i < NR_TTY;i++)
                {
                        
                        tty_read_kb(&tty_table[i]);
                        //tty_read_wrbuf(&tty_table[i]);
                        
                        tty_write_scr(&tty_table[i]);
                }
                
        }
}

TTY* get_current_TTY()
{
        return &tty_table[current_TTY];
}

void set_current_TTY(TTY *tty)
{
        if(current_TTY == tty->tty_num)
        {
                return;
        }
        
        current_TTY = tty->tty_num;
        u16 position = tty->tty_num * TTY_VMEM_SIZE / 2;
        //更改屏幕显示位置
        disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, position & 0xFF);
        enable_int();
        
        //清空按键缓冲
        //while(keyboard_read());
        
        set_cursor(position + tty->cur_rel);
       
}

void set_cursor(u32 pos)
{
        disable_int();
        out_byte(CRTC_ADDR_REG,CURSOR_H);
        out_byte(CRTC_DATA_REG,((pos)>>8) & 0xFF);
        out_byte(CRTC_ADDR_REG,CURSOR_L);
        out_byte(CRTC_DATA_REG,(pos) & 0xFF);
        enable_int();
}

void tty_read_kb(TTY *tty)
{
        if(tty->tty_num != current_TTY)
        {
                return;
        }

        u32 raw_key = keyboard_read();
 
        if((raw_key & FLAG_MAKE))//只处理make code 
        {
                
                if(!(raw_key & FLAG_EXT))//可打印字符的处理
                {
                        
                        tty->out_buf[tty->count] = raw_key & 0xFF;
                        tty->count++;
                }
                else 
                {
                        int key = raw_key & MASK_RAW;
                        switch(key)
                        {
                                case ENTER:  
                                {       /*
                                        一定要加这个大括号，在switch的一个case中是不允许直接
                                        声明一个变量的，应为这个case有可能不被执行，而后面可能
                                        会使用到这个变量，从而产生错误。加上大括号可以将变量的
                                        作用域限制到这个大括号中，也就不可能在其他地方使用这个变量
                                        了，所以这样不会出错
                                        */
                                        u32 remainer = tty->cur_rel % 80;
                                        u32 space_num = 80 - remainer;
                                        for(int i = 0;i < space_num;i++)
                                        {
                                                tty->out_buf[tty->count++] = ' ';
                                        }
                                }      
                                        break;
                                case BACKSPACE:
                                        tty->out_buf[tty->count++] = '\b';
                                        break;
                                case LEFT:
                                        
                                default:
                                        break;
                        }

                        if((raw_key & FLAG_CTRL_L) || (raw_key & FLAG_CTRL_R))
                        {
                                //ctrl + Fn 切换tty，用alt与系统快捷键有冲突
                                switch(key)
                                {
                                        case F1:
                                                set_current_TTY(&tty_table[0]);
                                                break;
                                        case F2:
                                                set_current_TTY(&tty_table[1]);
                                                break;
                                        case F3:
                                                set_current_TTY(&tty_table[2]);
                                                break;
                                        case F4:
                                                set_current_TTY(&tty_table[3]);
                                                break;
                                        case F5:
                                                set_current_TTY(&tty_table[4]);
                                                break;
                                        case F6:
                                                set_current_TTY(&tty_table[5]);
                                                break;
                                        case F7:
                                                set_current_TTY(&tty_table[6]);
                                                break;
                                        default:
                                                break;
                                }
                        }
                }               
        }
}

void tty_write_scr(TTY *tty)
{
        
        if(tty->count)
        {
                
                for(int i = 0;i < tty->count;i++)
                {
                        u8 *vmem = 0;
                        //如果是退格键
                        if(tty->out_buf[i] == '\b')
                        {
                                //如果光标不在开头
                                if(tty->cur_rel)
                                {
                                        tty->cur_rel--;
                                        vmem = tty->addr_start + tty->cur_rel * 2;
                                        *vmem++ = ' ';
                                        *vmem = 0x07;
                                }
                                
                                continue;
                        }
                        //如果是其他按键
                        vmem = tty->addr_start + tty->cur_rel * 2;
                        *vmem++ = tty->out_buf[i];
                        *vmem = 0x07;
                        tty->cur_rel++;
                }
                tty->count = 0;

                //说明显示到了屏幕末尾，要上滚一行
                if(tty->cur_rel == TTY_VMEM_SIZE / 2)
                {
                        u8 *vmem = tty->addr_start;
                        for(int i = 0;i < TTY_VMEM_SIZE - 80*2;i++)
                        {
                                 *vmem = *(vmem + 80*2);
                                 vmem++;
                        }
                        for(int i = TTY_VMEM_SIZE - 80*2;i < TTY_VMEM_SIZE;i += 2)
                        {
                                *vmem++ = ' ';
                                *vmem++ = 0x07;
                        }
                        tty->cur_rel = 80 * 24;
                }
                u16 position = (tty->cur_rel + tty->tty_num * TTY_VMEM_SIZE / 2);
                set_cursor(position);  
        }    
}

void tty_read_wrbuf(TTY *tty)
{
        int ttynum = tty->tty_num;
        if(wr_buf[ttynum].is_ready)
        {
                wr_buf[ttynum].is_reading = 1;
                for(int i = 0;i < wr_buf[ttynum].len;i++)
                {
                        tty->out_buf[tty->count] = wr_buf[ttynum].buf[i];
                        tty->count++;
                }
                wr_buf[ttynum].is_reading = 0;
                wr_buf[ttynum].is_ready =0;
        } 
}
