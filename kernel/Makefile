CC      = owcc
AS      = nasm
CFLAGS  = -O2 -Wall -Wextra

C_FILES     := $(shell find . -type f -name '*.c' | sort)
ASM_FILES   := $(shell find . -type f -name '*.asm' | sort)
REAL_FILES  := $(shell find . -type f -name '*.real' | sort)
OBJ         := $(ASM_FILES:.asm=.obj) $(C_FILES:.c=.obj)
BINS        := $(REAL_FILES:.real=.bin)

CHARDFLAGS  := -std=c99 -fpack-struct=1 -fno-stack-check -I.

phoenux.bin: $(BINS) $(OBJ)
	echo 'name phoenux.bin'                >  linker
	echo 'output raw'                      >> linker
	echo '    offset=0x100000'             >> linker
	echo 'option nodefaultlibs'            >> linker
	for i in $(OBJ); do printf 'file %s\n' $$i >> linker; done
	echo 'order'                           >> linker
	echo '    clname STRT offset=0x100000' >> linker
	echo '    clname CODE'                 >> linker
	echo '    clname DATA'                 >> linker
	wlink                                  <  linker
	rm linker

%.obj: %.c
	$(CC) $(CHARDFLAGS) $(CFLAGS) -c $< -o $@

%.bin: %.real
	$(AS) $< -f bin -o $@

%.obj: %.asm
	$(AS) $< -f obj -o $@

clean:
	rm -f $(OBJ) $(BINS) phoenux.bin
