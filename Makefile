# Automatically generate lists of sources using wildcards
C_SOURCES = $(wildcard utilities/*.c  kernel/*.c kernel/process/*.c kernel/memory/*.c  kernel/cpu/*.c kernel/terminal/*.c kernel/screen/*.c kernel/bios/*.c) 
O_src = $(wildcard utilities/*.o kernel/*.o kernel/cpu/*.o kernel/process/*.o  kernel/memory/*.o  kernel/terminal/*.o)
#
#

INC_DIR = .
# The option -ffreestanding directs the compiler 
# to not assume that standard functions 
# have their usual definition
CFLAGS=   -fno-pic -fno-pie -fno-exceptions -Wno-multichar -Wno-int-conversion -Wno-implicit-function-declaration -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs  -m32 -g -c  -I include/

#CLANGFLAGS = --target=i386-pc-none-elf  -mcmodel=small -I include/ -Wall -Wno-pointer-sign -Wno-unused-variable -Wno-unused-function  -Wnoimplicit-function-declaration


# Convert the *.c filenames to *.o to give a list of object files to build
OBJ = ${C_SOURCES:.c=.o }
OBJ2 = ${kernel/kernel.c:.c=.o }

# Generic rule for compiling C code to an object file
# The compiler outputs annotated machine code,
# where meta information, such as textual labels, 
# that are redundant for execution, 
# remain present to enable more flexibilty 
# in how the code is eventually put together.
# One big advantage of this intermediary format 
# is that the code may be easily relocated into a larger binary file
# when linked in with routines from other routines in other libraries, 
# since code in the object file uses relative 
# rather than absolute internel memory references.
# $< is the first dependancy and $@ is the target file
%.o: %.c
	gcc ${CFLAGS} -c $< -o $@ -fpermissive
	
	#clang ${CLANGFLAGS} -c $< -o $@

# The option -f elf tells the assembler 
# to output an object file of the particular format Executable 
# and Linking Format (ELF), 
# which is the default format output by out C compiler
%.o: %.asm
	nasm $< -f elf -o $@

# In order to create the actual executable code
# we have to use a linker, 
# whose role is to link together all of the routines 
# described in the input object files 
# into one executable binary file, 
# effectively stitching them together 
# and converting those relative addresses 
# into absolute addresses within the aggregated machine code

# The -Ttext option puts the .text section of your program by the given address.

# the linker can output executable files in various formats
# some of which may retain meta data from the input object files.
# meta data can be retained to describe 
# how those applications are 
# to be loaded into memory; 
# and for debugging purposes
# Anyhow, since we are interested in writing an operating system,
# CPU is unware of metadata and will execute every byte as machine code
# This is why we specify an output format of (raw) binary

# This builds the binary of our kernel from two object files:
# the kernel_entry, which jumps to main() in our kernel
# the compiled C kernel
# Build the kernel binary
# The final ELF will have following format .text - .rodata - .data
# $^ is substituted with all of the target's dependancy files


kernel.bin: kernel/kernel_entry.o kernel/cpu/interrupt_routines.o  ${OBJ} 
	ld -m elf_i386 -o $@ -T linker.ld  $^ --oformat binary
	#clang -target i386-pc-none-elf  -T linker.ld $^ -o  $@  -g3


kernel.out: kernel.elf
	objcopy -O binary kernel.elf kernel.out

# Used for debugging purposes
kernel.elf: kernel/kernel_entry.o kernel/cpu/interrupt_routines.o ${OBJ} 
	ld -m elf_i386 -o $@ -Ttext 0xA000 $^
	#ld -m elf_i386 -o $@ -T linker.ld $^


# Assemble the boot sector to raw machine code
# The -I options tells nasm where to find our useful assembly
# routines that we include in boot_sect . asm
boot_sect.bin: ./boot/d.asm
#boot_sect.bin: ./boot/bootsect.asm
	nasm $< -f bin -i ./boot/ -o $@


# This is the actual disk image that the computer loads ,
# which is the combination of our compiled bootsector and kernel
os.img: boot_sect.bin  kernel.out 
	cat $^ > $@

all: os.img


debug: kernel.dis

# Disassemble our kernel - might be useful for debugging
kernel.dis: kernel.bin
	ndisasm -b 32 $< > $@

# Clear away all generated files .
clean:
	#rm ${OBJ}
	#rm -fr *.bin *.dis *.o os.img *.map *.elf *.dis *.buf 
	find  . -name \*.o -type f -delete
	find  . -name \*.bin -type f -delete
	find  . -name \*.elf -type f -delete
	find  . -name \*.buf -type f -delete
	rm os.img

# Run bochs to simulate booting of our code
debug3: all kernel.elf
	qemu-system-i386 -s -vnc :0 -fda os.img -d guest_errors,int & gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"
