#include <sys/pic_8259.h>
#include <sys/gdt.h>
#include <drv/vga_textmode.h>
#include <lib/kprint.h>

void kernel_init(void) {
    pic_8259_mask_all();
    pic_8259_remap(0x20, 0x28);

    load_gdt();
    load_tss();

    init_vga_textmode();

    kprint(KPRN_INFO, "Welcome to Phoenux");

    for (;;) {}
}
