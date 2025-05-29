target/boot/boot.bin: src/boot/boot.asm src/boot/print.asm src/boot/disk.asm
	nasm -f bin src/boot/boot.asm -o target/boot/boot.bin

target/boot/loader.bin: src/boot/loader.asm src/boot/print.asm src/boot/disk.asm
	nasm -f bin src/boot/loader.asm -o target/boot/loader.bin

target/master.img: target/boot/boot.bin target/boot/loader.bin
	(echo yes) | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat target/master.img      
	dd if=target/boot/boot.bin of=target/master.img bs=512 count=1
	dd if=target/boot/loader.bin of=target/master.img bs=512 count=4 seek=2

.PHONY: clean
clean:
	del /q target\*
	for /d %%i in (target\*) do rd /s /q "%%i"

.PHONY: bochs
bochs: target/master.img
	bochs -q -f bochsrc

.PHONY: bochsdbg
bochsdbg: target/master.img
	bochsdbg -q -f bochsrc_gui