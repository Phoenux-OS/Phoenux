CC      = i686-phoenux-gcc
AS      = nasm
OBJCOPY = i686-phoenux-objcopy
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

phoenux.bin: $(BINS) $(OBJ)
	$(CC) $(OBJ) $(LDHARDFLAGS) $(LDFLAGS) -o phoenux.elf
	$(OBJCOPY) -O binary phoenux.elf phoenux.bin

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
