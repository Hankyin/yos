#向内核中添加一个编译单元
#在变量HEAD_KERNEL中添加头文件
#在变量SRC_KERNEL_C中添加源文件
#在变量OBJ_KERNEL中添加目标文件
#添加该文件的编译指令


#内核程序入口点
K_ENTRY_POS 	= 0x30400

#编译器设置
ASM 		= nasm
ASMFLAGS 	= -f elf -I $(DIR_BOOT)/
CC 		= gcc 
CCFLAGS		= -c -fno-builtin -m32 -I $(DIR_LIB)/ -I $(DIR_K_INCLUDE)/
LD 		= ld 
LDFLAGS 	= -s -m elf_i386 -Ttext $(K_ENTRY_POS)
OUT		= -o $(DIR_BUILD)/
#目录设置
DIR_BOOT 	= ./boot
DIR_KERNEL 	= ./kernel
DIR_K_INCLUDE	= ./kernel/include
DIR_LIB 	= ./lib
DIR_BUILD 	= ./build

#头文件
HEAD_KERNEL	= $(DIR_K_INCLUDE)/gdt.h $(DIR_K_INCLUDE)/idt.h $(DIR_K_INCLUDE)/global.h \
		$(DIR_K_INCLUDE)/clock.h $(DIR_K_INCLUDE)/irq.h $(DIR_K_INCLUDE)/process.h \
		$(DIR_K_INCLUDE)/keyboard.h $(DIR_K_INCLUDE)/keymap.h $(DIR_K_INCLUDE)/tty.h \
		$(DIR_K_INCLUDE)/syscall.h $(DIR_K_INCLUDE)/yos.h \
		$(HEAD_LIB)
HEAD_LIB	= $(DIR_LIB)/lib.h $(DIR_LIB)/type.h $(DIR_LIB)/x86.h 

#源文件
SRC_BOOT	= $(DIR_BOOT)/boot.asm $(DIR_BOOT)/fat12hdr.inc
SRC_LOADER	= $(DIR_BOOT)/loader.asm $(DIR_BOOT)/fat12hdr.inc $(DIR_BOOT)/pm.inc 
SRC_KERNEL_C	= $(DIR_BUILD)/gdt.c $(DIR_BUILD)/idt.c $(DIR_BUILD)/irq.c \
		$(DIR_BUILD)/clock.c $(DIR_BUILD)/process.c $(DIR_BUILD)/keyboard.c \
		$(DIR_BUILD)/tty.c $(DIR_BUILD)/syscall.c
SRC_KERNEL_ASM	= $(DIR_BUILD)/kernel.asm $(DIR_LIB)/liba.asm $(DIR_BUILD)/yos.asm 
#obj文件
OBJ_KERNEL	= $(DIR_BUILD)/kernel.o $(DIR_BUILD)/gdt.o $(DIR_BUILD)/idt.o \
		$(DIR_BUILD)/irq.o $(DIR_BUILD)/clock.o $(DIR_BUILD)/process.o \
		$(DIR_BUILD)/tty.o $(DIR_BUILD)/keyboard.o $(DIR_BUILD)/yos.o \
		$(DIR_BUILD)/syscall.o \
		$(OBJ_LIB)
OBJ_LIB		= $(DIR_BUILD)/liba.o $(DIR_BUILD)/libc.o 
#目标
BOOT		= $(DIR_BUILD)/boot.bin
LOADER		= $(DIR_BUILD)/loader.bin
KERNEL		= $(DIR_BUILD)/kernel.bin
#伪目标
.PHONY:clean all bochs

all:clean $(BOOT) $(LOADER) $(KERNEL)
	
bochs:a.img
	bochs
a.img:all 
	dd if=$(DIR_BUILD)/boot.bin of=a.img bs=512 count=1 conv=notrunc
	sudo mount -o loop a.img /mnt/
	sudo cp -fv $(DIR_BUILD)/loader.bin /mnt/
	sudo cp -fv $(DIR_BUILD)/kernel.bin /mnt/
	sudo umount /mnt/
$(BOOT):$(SRC_BOOT)
	$(ASM) -I $(DIR_BOOT)/ $(DIR_BOOT)/boot.asm $(OUT)boot.bin

$(LOADER):$(SRC_LOADER)
	$(ASM) -I $(DIR_BOOT)/ $(DIR_BOOT)/loader.asm $(OUT)loader.bin
	
$(KERNEL):$(OBJ_KERNEL) $(HEAD_KERNEL) $(HEAD_LIB)
	$(LD) $(LDFLAGS) $(OBJ_KERNEL) $(OUT)kernel.bin 


#编译kernel
$(DIR_BUILD)/kernel.o:$(DIR_KERNEL)/kernel.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(DIR_BUILD)/yos.o:$(DIR_KERNEL)/yos.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(DIR_BUILD)/gdt.o:$(DIR_KERNEL)/gdt.c
	$(CC) $(CCFLAGS) $< -o $@

$(DIR_BUILD)/idt.o:$(DIR_KERNEL)/idt.c
	$(CC) $(CCFLAGS) $< -o $@

$(DIR_BUILD)/irq.o:$(DIR_KERNEL)/irq.c
	$(CC) $(CCFLAGS) $< -o $@

$(DIR_BUILD)/clock.o:$(DIR_KERNEL)/clock.c
	$(CC) $(CCFLAGS) $< -o $@

$(DIR_BUILD)/process.o:$(DIR_KERNEL)/process.c
	$(CC) $(CCFLAGS) $< -o $@

$(DIR_BUILD)/keyboard.o:$(DIR_KERNEL)/keyboard.c
	$(CC) $(CCFLAGS) $< -o $@

$(DIR_BUILD)/tty.o:$(DIR_KERNEL)/tty.c
	$(CC) $(CCFLAGS) $< -o $@

$(DIR_BUILD)/syscall.o:$(DIR_KERNEL)/syscall.c
	$(CC) $(CCFLAGS) $< -o $@

#编译lib
$(DIR_BUILD)/libc.o:$(DIR_LIB)/libc.c
	$(CC) $(CCFLAGS) $< -o $@
$(DIR_BUILD)/liba.o:$(DIR_LIB)/liba.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf $(DIR_BUILD)/*