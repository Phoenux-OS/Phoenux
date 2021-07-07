.PHONY: all
all: phoenux.img

.PHONY: distro
distro:
	mkdir -p build 3rdparty
	cd build && ln -s ../sysroot system-root && xbstrap init ..
	cd build && xbstrap install --all

.PHONY: kernel/phoenux.bin
kernel/phoenux.bin:
	cd build && xbstrap install --rebuild kernel

phoenux.img: kernel/phoenux.bin
	cp ./build/tools/host-limine/share/limine/limine.sys kernel/phoenux.bin sysroot/boot/
	./dir2echfs -f phoenux.img 64 sysroot
	./build/tools/host-limine/bin/limine-install phoenux.img

run: phoenux.img
	qemu-system-x86_64 -net none -m 2G -serial stdio -enable-kvm -hda phoenux.img

run-nokvm: phoenux.img
	qemu-system-x86_64 -monitor stdio -net none -m 2G -hda phoenux.img

.PHONY: clean
clean:
	rm -f phoenux.img
	$(MAKE) -C kernel clean

.PHONY: distclean
distclean: clean
	rm -rf 3rdparty build kernel/*.xbstrap
	rm -rf sysroot/usr sysroot/etc/xbstrap sysroot/boot/limine.sys sysroot/boot/phoenux.bin
