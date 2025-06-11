BUILD:=./target
SRC:=./src

ENTRYPONIT:=0x10000

CFLAGS:= -m32 # 32位程序
CFLAGS+= -fno-builtin # 不需要gcc内置函数
CFLAGS+= -nostdinc  # 不需要标准头文件
CFLAGS+= -fno-pic   # 不需要位置无关的代码
CFLAGS+= -fno-pie   # 不需要位置无关的可执行程序
CFLAGS+= -nostdlib  # 不需要标准库
CFLAGS+= -fno-stack-protector # 不需要栈保护
CFLAGS:=$(strip ${CFLAGS})

DEBUG:= -g
INCLUDE:=-I$(SRC)/include

${BUILD}/boot/boot.bin: ${SRC}/boot/boot.asm ${SRC}/boot/print.asm ${SRC}/boot/disk.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin src/boot/boot.asm -o $@

${BUILD}/boot/loader.bin: ${SRC}/boot/loader.asm ${SRC}/boot/print.asm ${SRC}/boot/disk.asm ${SRC}/boot/memory.asm ${SRC}/boot/gdt.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin src/boot/loader.asm -o $@

$(BUILD)/kernel/%.o: ${SRC}/kernel/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $^ -o $@

$(BUILD)/kernel/%.o: ${SRC}/kernel/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c  $< -o $@

$(BUILD)/lib/%.o: ${SRC}/lib/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAGS) $(DEBUG) $(INCLUDE) -c  $< -o $@

$(BUILD)/kernel.bin: ${BUILD}/kernel/start.o \
	$(BUILD)/kernel/main.o \
	$(BUILD)/kernel/io.o \
	$(BUILD)/kernel/console.o \
	$(BUILD)/kernel/printk.o \
	$(BUILD)/kernel/assert.o \
	$(BUILD)/lib/string.o \
	$(BUILD)/lib/vsprintf.o
	$(shell mkdir -p $(dir $@))
	ld -m elf_i386 -static $^ -o $@ -Ttext $(ENTRYPONIT)

$(BUILD)/system.bin: $(BUILD)/kernel.bin
	objcopy -O binary $< $@

$(BUILD)/system.map: $(BUILD)/kernel.bin
	nm $^ | sort > $@ 

${BUILD}/master.img: ${BUILD}/boot/boot.bin ${BUILD}/boot/loader.bin ${BUILD}/system.bin ${BUILD}/system.map
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@    
	dd if=${BUILD}/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=${BUILD}/boot/loader.bin of=$@ bs=512 count=4 seek=2 conv=notrunc
	dd if=${BUILD}/system.bin of=$@ bs=512 count=200 seek=10 conv=notrunc

test: ${BUILD}/master.img

.PHONY: clean
clean:
	rm -rf $(BUILD)

.PHONY: bochs
bochs: ${BUILD}/master.img
	bochs -q -f ./bochs/bochsrc

.PHONY: bochsg
bochsg: ${BUILD}/master.img
	bochs-gdb -q -f ./bochs/bochsrc.gdb