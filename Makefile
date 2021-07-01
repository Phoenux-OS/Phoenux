CUR_DIR := $(shell pwd)
PATH    := $(CUR_DIR)/toolchain/cross-root/bin:$(PATH)
SYSROOT := $(CUR_DIR)/root

.PHONY: all clean run

all: phoenux.img

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1
	make -C limine

root/phoenux.bin:
	$(MAKE) -C kernel
	mkdir -p root/boot
	cp kernel/phoenux.bin root/boot/

clean:
	$(MAKE) clean -C kernel
	rm -f root/phoenux.bin

phoenux.img: limine root/phoenux.bin
	mkdir -p root/boot
	cp limine/limine.sys root/boot/
	./dir2echfs -f phoenux.img 64 root
	limine/limine-install phoenux.img

run: phoenux.img
	qemu-system-x86_64 -monitor stdio -net none -m 2G -enable-kvm -hda phoenux.img

run-nokvm: phoenux.img
	qemu-system-x86_64 -monitor stdio -net none -m 2G -hda phoenux.img

# Various ports

bash:
	$(MAKE) -C ports/bash
	$(MAKE) -C ports/bash DESTDIR=$(SYSROOT) install
