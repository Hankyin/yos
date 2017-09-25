#include "keyboard.h"
#include "idt.h"
#include "lib.h"
#include "x86.h"
#include "keymap.h"

static KB_INPUT kb_in;//键盘输入缓冲区
//功能键的状态
static int shift_l;
static int shift_r;
static int ctrl_l;
static int ctrl_r;
static int alt_l;
static int alt_r;

static int caps_lk;
static int num_lk;
static int scr_lk;

static int code_with_E0;

static int column;

static u8 get_byte_from_kbuf();
void keyboard_handler(int irq)
{
	u8 scan_code = in_byte(0x60);
	int scan = scan_code;
        //print_int(scan,PCOLOR_RED);
        if(kb_in.count < KB_IN_BYTES)
        {
                *(kb_in.p_head) = scan_code;
                kb_in.p_head++;
                if(kb_in.p_head == kb_in.buf + KB_IN_BYTES)
                {
                        kb_in.p_head = kb_in.buf;
                }
                kb_in.count++;
        }
}

void init_kerbord()
{
        kb_in.count = 0;
        kb_in.p_head = kb_in.buf;
        kb_in.p_tail = kb_in.buf;
        
	shift_l = shift_r = 0;
	ctrl_l = ctrl_r = 0;
        alt_l = alt_r = 0;
        
        caps_lk = 0;
        num_lk = 1;
        scr_lk = 0;
        set_led();

        put_irq_handler(KEYBOARD_IRQ,keyboard_handler);
        enable_irq(KEYBOARD_IRQ);
}


u32 keyboard_read()
{
        
        u8 scan_code;
        
        int is_mkcode;
        u32 key = 0;
        /*
                u32 key各位的作用
                0 - 8  按键ascii码
                9      非打印字符标志
                10      shift_l
                11      shift_r
                12      ctrl_l
                13      ctrl_r
                14      alt_l
                15      alt_r
                16      numpad

                30      is_mkcode
                31      有按键标志
        */
                        
        u32 *keyrow = 0;//指向键盘映射中的一行
        
        if(kb_in.count > 0)
        {
                code_with_E0 = 0;
                scan_code = get_byte_from_kbuf();

                //解析扫描码
                if(scan_code == 0xE1)
                {       
                      
                        //pause没有break code
                        u8 pause_code[] = {0xE1,0x1D,0x45,0xE1,0x9D,0xC5};
                        int is_pause = 1;

                        //注意此时要从1开始循环，因为第一个code已经读取了
                        for(int i = 1;i < 6;i++)
                        {
                                if(get_byte_from_kbuf() != pause_code[i])
                                {
                                        is_pause = 0;
                                        break;
                                }
                        }
                        if(is_pause)
                                key = PAUSEBREAK;
                }
                else if(scan_code == 0xE0)
                {
                      
                        scan_code = get_byte_from_kbuf();
                         //检测printscreen，
                        if(scan_code == 0x2A)
                        {
                                if(get_byte_from_kbuf() == 0xE0)
                                {
                                        if(get_byte_from_kbuf() == 0x37)
                                        {
                                                key = PRINTSCREEN;
                                                is_mkcode = 1;
                                        }
                                }
                        }

                        if(scan_code == 0xB7)
                        {
                                if(get_byte_from_kbuf() == 0xE0)
                                {
                                        if(get_byte_from_kbuf() == 0xAA)
                                        {
                                                key == PRINTSCREEN;
                                                is_mkcode = 0;
                                        }
                                }
                        }

                        if(key == 0)//如果不是printscreen
                        {
                                code_with_E0 = 1;
                        }
                }
                
                if((key != PAUSEBREAK) && (key != PRINTSCREEN)) //下面处理一个字节的扫描码和E0后的扫描码
                {
                        /*
                          判断makecode和 break code 
                          break code是make code和0x80或操作的结果（1000 0000）
                          通过观察，易得：make code 高8位小于8，break code 则大于8
                          也就是说，make code最高位为0，break 最高位为1。
                          由此来区分两种code
                        */
                        is_mkcode = (scan_code & 0x80) ? 0 : 1;
			
                        //定位到keymap中的一行
                        keyrow = &keymap[(scan_code & 0x7F) * MAP_COLS];
                        column = 0;

                        int caps = shift_l || shift_r;
                        if(caps_lk)
                        {
                                if((keyrow[0] >= 'a') && (keyrow[0] <= 'z'))
                                {
                                        caps = !caps;
                                }
                        }
                        if(caps)
                        {
                                column = 1;
                        }
                        if(code_with_E0)
                        {
                                column = 2;
                                code_with_E0 = 0;
                        }
			key = keyrow[column];
			//
                        switch(key)
                        {
                                case SHIFT_L:
                                        shift_l = is_mkcode;
                                        break;
                                case SHIFT_R:
                                        shift_r = is_mkcode;
                                        break;
                                case CTRL_L:
                                        ctrl_l = is_mkcode;
                                        break;
                                case CTRL_R:
                                        ctrl_r = is_mkcode;
                                        break;
                                case ALT_L:
                                        alt_l = is_mkcode;
                                        break;
                                case ALT_R:
                                        alt_r = is_mkcode;
                                        break;
                                case CAPS_LOCK:
                                        if(is_mkcode)
                                        {
                                                caps_lk = !caps_lk;
                                                set_led();
                                        }
                                        break;
                                case NUM_LOCK:
                                        if(is_mkcode)
                                        {
                                                num_lk = !num_lk;
                                                set_led();
                                        }
                                        break;
                                case SCROLL_LOCK:
                                        if(is_mkcode)
                                        {
                                                scr_lk = !scr_lk;
                                                set_led();
                                        }
                                        break;
                                default:
                                        break;
                        }
                        
                        //给key的各个标志位赋值
                        key |= shift_l	? FLAG_SHIFT_L	: 0;
                        key |= shift_r	? FLAG_SHIFT_R	: 0;
                        key |= ctrl_l	? FLAG_CTRL_L	: 0;
                        key |= ctrl_r	? FLAG_CTRL_R	: 0;
                        key |= alt_l	? FLAG_ALT_L	: 0;
                        key |= alt_r	? FLAG_ALT_R	: 0;

                        key |= is_mkcode ? FLAG_MAKE    : 0;
   
                }

        }
        
        return key;
}

static u8 get_byte_from_kbuf()
{
        
        u8 scan_code;
        while(kb_in.count <= 0);//等待用户输入

        __asm__("cli");
        scan_code = *(kb_in.p_tail);//tail中保存的当前要处理的扫描码
        kb_in.p_tail++;
        if(kb_in.p_tail == kb_in.buf + KB_IN_BYTES)
        {
                kb_in.p_tail = kb_in.buf; //tail到达缓冲区尾部时让其回到开头
        }
        kb_in.count--;
        __asm__("sti");

        return scan_code;
}

void set_led()
{
        u8 led_state = (caps_lk << 2) | (num_lk << 1) | scr_lk;

        u8 kb_stat;
        u8 kb_read;
        
        do {
                kb_stat = in_byte(KB_CMD);
        } while (kb_stat & 0x02);

        out_byte(KB_DATA, LED_CODE);

        do {
                kb_read = in_byte(KB_DATA);
        } while (kb_read =! KB_ACK);

        do {
                kb_stat = in_byte(KB_CMD);
        } while (kb_stat & 0x02);

        out_byte(KB_DATA, led_state);

        do {
                kb_read = in_byte(KB_DATA);
        } while (kb_read =! KB_ACK);
}