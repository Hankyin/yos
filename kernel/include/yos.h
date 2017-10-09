/*
系统头文件，包含了用户程序可用的系统调用
*/

int get_ticks();

void write(char *buf,size_t len);//向屏幕输出字符

int get_char();//获取当前输入的按键