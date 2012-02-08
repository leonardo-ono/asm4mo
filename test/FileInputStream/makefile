all: exec

exec: save
	qemu -fda ~/mikeos-4.2/disk_images/mikeos.flp

save: asm
	sudo mount ~/mikeos-4.2/disk_images/mikeos.flp /mnt/mikeos -o loop
	sudo rm /mnt/mikeos/test.bin -f
	sudo rm /mnt/mikeos/TESTE.ASM -f
	sudo cp test.bin /mnt/mikeos
	sudo cp TESTE.ASM /mnt/mikeos
	sudo umount /mnt/mikeos

asm: clear
	nasm -o test.bin FileInputStreamTest.asm

clear:
	rm *.bin -f

