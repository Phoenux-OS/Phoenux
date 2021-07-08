CC      = owcc
AS      = nasm
CFLAGS  = -O2 -Wall -Wextra

C_FILES     := $(shell find . -type f -name '*.c' | sort)
ASM_FILES   := $(shell find . -type f -name '*.asm' | sort)
OBJ         := $(ASM_FILES:.asm=.obj) $(C_FILES:.c=.obj)

CHARDFLAGS  := -std=c99 -fpack-struct=1 -fno-stack-check -I. -I${WATCOM}/h

phoenux.bin: $(BINS) $(OBJ)
	echo 'name phoenux.bin'               				>  linker
	echo 'output raw'                      				>> linker
	echo '    offset=0x200000'            			    >> linker
	echo 'option nodefaultlibs,start=kernel_init_'      >> linker
	for i in $(OBJ); do printf 'file %s\n' $$i			>> linker; done
	echo 'order'                           				>> linker
	echo '    clname CODE offset=0x200000'        		>> linker
	echo '    clname DATA'                 				>> linker
	wlink                                  				<  linker
	rm linker

%.obj: %.c
	$(CC) $(CHARDFLAGS) $(CFLAGS) -c $< -o $@

%.obj: %.asm
	$(AS) $< -f obj -o $@

clean:
	rm -f $(OBJ) phoenux.bin
