#include "irq.h"
#include "type.h"
#include "keyboard.h"
u32 		k_reenter;	//中断重入

void init_irq()
{
        init_clock();
        init_kerbord();

}