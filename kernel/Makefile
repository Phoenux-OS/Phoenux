CC      = i686-phoenux-gcc
AS      = nasm
PREFIX  = /
CFLAGS  = -O2 -Wall -Wextra
LDFLAGS =

C_FILES     := $(shell find . -type f -name '*.c')
ASM_FILES   := $(shell find . -type f -name '*.asm')
REAL_FILES  := $(shell find . -type f -name '*.real')
OBJ         := $(C_FILES:.c=.o) $(ASM_FILES:.asm=.o)
BINS        := $(REAL_FILES:.real=.bin)

CHARDFLAGS  := -std=gnu99 -masm=intel -fno-pic -mno-sse -mno-sse2 -ffreestanding -fno-stack-protector -I.
LDHARDFLAGS := -no-pie -nostdlib -lgcc -static-libgcc -T ./linker.ld

phoenux.bin: phoenux.elf
	./gensyms.sh
	$(CC) -x c $(CHARDFLAGS) $(CFLAGS) -c symlist.gen -o symlist.o
	$(CC) $(OBJ) symlist.o $(LDHARDFLAGS) -Wl,--oformat=binary $(LDFLAGS) -o $@

phoenux.elf: $(BINS) $(OBJ) symlist.o
	$(CC) $(OBJ) symlist.o $(LDHARDFLAGS) $(LDFLAGS) -o $@

symlist.o:
	echo '#include <symlist.h>' > symlist.gen
	echo 'struct symlist_t symlist[] = {{0xffffffff,""}};' >> symlist.gen
	$(CC) -x c $(CHARDFLAGS) $(CFLAGS) -c symlist.gen -o symlist.o

%.o: %.c
	$(CC) $(CHARDFLAGS) $(CFLAGS) -c $< -o $@

%.bin: %.real
	$(AS) $< -f bin -o $@

%.o: %.asm
	$(AS) $< -f elf32 -o $@

clean:
	rm -f $(OBJ) $(BINS) phoenux.bin

install:
	mkdir -p $(PREFIX)
	cp phoenux.bin $(PREFIX)/
