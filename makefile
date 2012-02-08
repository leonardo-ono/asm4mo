#all: exec
#
#exec: save
#	qemu -fda /home/leo/mikeos-4.2/disk_images/mikeos.flp
#
#save: asm
#	sudo mount /home/leo/mikeos-4.2/disk_images/mikeos.flp /mnt/mikeos -o loop
#	sudo rm /mnt/mikeos/asm.bin -f
#	sudo rm /mnt/mikeos/TEST.ASM -f
#	sudo cp asm.bin /mnt/mikeos
#	sudo cp examples/TEST.ASM /mnt/mikeos
#	sudo umount /mnt/mikeos

asm: clear
	nasm -o asm.bin asm.asm

clear:
	rm *.bin -f

