CUR_DIR := $(shell pwd)
PATH    := $(CUR_DIR)/toolchain/cross-root/bin:$(PATH)
SYSROOT := $(CUR_DIR)/root

all: phoenux.img

root/phoenux.bin:
	$(MAKE) -C kernel
	$(MAKE) -C kernel PREFIX=$(SYSROOT) install

clean:
	$(MAKE) clean -C kernel
	rm -f root/phoenux.bin

phoenux.img: root/phoenux.bin
	nasm bootloader/bootloader.asm -f bin -o phoenux.img
	dd bs=32768 count=0 seek=8192 if=/dev/zero of=phoenux.img
	echfs-utils phoenux.img format 512
	./copy-root-to-img.sh root phoenux.img
