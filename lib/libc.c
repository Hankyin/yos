/*
库函数：C语言实现部分
*/

#include "lib.h"
#include "type.h"
#include "global.h"

/*C语言常用库函数的实现*/
char* itoa(char *str,int num)
{
    if(str == 0)
        return 0;
    char *index = "0123456789ABCDEF";
    char tmp[11];
    u8 last_num = 0;

    for(int i = 0;i < 8;i ++)
    {
        last_num = num % 16;
        num >>= 4;
        tmp[i] = index[last_num];
    }
    tmp[8] = 'x';
    tmp[9] = '0';
    for(int i = 0;i < 10;i++)
    {
        str[i] = tmp[9 - i];
    }
    return str;
}

size_t strlen(const char *s)
{
    const char *sc;
    for(sc = s;*sc != '\0';++sc)
        ;
    return (sc - s);
}

char *strcpy(char* dest, const char* src)
{
    if((dest == 0) || (src == 0))
        return 0;
    char *dest_bk = dest;
    while((*dest++ = *src++) != '\n');
    return dest_bk;
}

/*
void delay(u32 m_sec)
{
    int t = get_ticks();
    while(((get_ticks() - t) * 1000 /HZ) < m_sec);
}
*/
void print_int(int num, u8 color)
{
    char str[11];// 一个int会转换成10个字符，所以需要11个长度的字符串。
    itoa(str,num);
    print_str(str,color);
}

/*由段名求绝对地址*/
u32 seg2phys(u16 seg)
{
	DESCRIPTOR* p_dest = &gdt[seg >> 3];
	return (p_dest->base_high<<24 | p_dest->base_mid<<16 | p_dest->base_low);
}

/* 初始化段描述符*/
 void init_descriptor(DESCRIPTOR *p_desc,u32 base,u32 limit,u16 attribute)
{
	p_desc->limit_low	= limit & 0x0FFFF;
	p_desc->base_low	= base & 0x0FFFF;
	p_desc->base_mid	= (base >> 16) & 0x0FF;
	p_desc->attr1		= attribute & 0xFF;
	p_desc->limit_high_attr2= ((limit>>16) & 0x0F) | (attribute>>8) & 0xF0;
	p_desc->base_high	= (base >> 24) & 0x0FF;
}

void disable_int()
{
    __asm__("cli");
}

void enable_int()
{
    __asm__("sti");
}

   