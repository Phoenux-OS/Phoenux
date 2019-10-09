CC = i686-phoenux-gcc
PREFIX = 

C_FILES = $(shell find . -type f -name '*.c')
H_FILES = $(shell find . -type f -name '*.h')
ASM_FILES = $(shell find . -type f -name '*.asm')
REAL_FILES = $(shell find . -type f -name '*.real')
OBJ = $(C_FILES:.c=.o) $(ASM_FILES:.asm=.o)
BINS = $(REAL_FILES:.real=.bin)

CHARDFLAGS = -std=gnu99 -masm=intel -fno-pic -mno-sse -mno-sse2 -ffreestanding -fno-stack-protector -I.
CFLAGS = -O2 -Wall -Wextra
LDHARDFLAGS = -no-pie -nostdlib -lgcc -static-libgcc
LDFLAGS = -O2

phoenux.bin: $(BINS) $(OBJ) $(H_FILES)
	$(CC) $(OBJ) $(LDHARDFLAGS) $(LDFLAGS) -T ./linker.ld -o $@

%.o: %.c
	$(CC) $(CHARDFLAGS) $(CFLAGS) -c $< -o $@

%.bin: %.real
	nasm $< -f bin -o $@

%.o: %.asm
	nasm $< -f elf32 -o $@

clean:
	rm -f $(OBJ) $(BINS) phoenux.bin

install:
	cp phoenux.bin $(PREFIX)/
