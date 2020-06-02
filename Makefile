CUR_DIR := $(shell pwd)
PATH    := $(CUR_DIR)/toolchain/cross-root/bin:$(PATH)
SYSROOT := $(CUR_DIR)/root

.PHONY: all clean run

all: phoenux.img

root/phoenux.bin:
	$(MAKE) -C kernel
	mkdir -p root
	cp kernel/phoenux.bin root/

clean:
	$(MAKE) clean -C kernel
	rm -f root/phoenux.bin

phoenux.img: root/phoenux.bin
	nasm bootloader/bootloader.asm -f bin -o phoenux.img
	dd bs=32768 count=0 seek=8192 if=/dev/zero of=phoenux.img
	echfs-utils phoenux.img format 512
	./copy-root-to-img.sh root phoenux.img

run: phoenux.img
	qemu-system-x86_64 -monitor stdio -net none -m 2G -enable-kvm -hda phoenux.img

run-nokvm: phoenux.img
	qemu-system-x86_64 -monitor stdio -net none -m 2G -hda phoenux.img

# Various ports

bash:
	$(MAKE) -C ports/bash
	$(MAKE) -C ports/bash DESTDIR=$(SYSROOT) install
