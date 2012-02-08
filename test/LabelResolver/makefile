all: exec

exec: save
	qemu -fda ~/mikeos-4.2/disk_images/mikeos.flp

save: asm
	sudo mount ~/mikeos-4.2/disk_images/mikeos.flp /mnt/mikeos -o loop
	sudo rm /mnt/mikeos/test.bin -f
	sudo cp test.bin /mnt/mikeos
	sudo umount /mnt/mikeos

asm: clear
	nasm -o test.bin LabelResolverTest.asm

clear:
	rm *.bin -f

