fs:
	gcc inode.c buf.c disk_emulator.c main.c dir.c calls.c -I. -o fs.elf
