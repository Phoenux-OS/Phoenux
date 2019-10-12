#include <sys/pic_8259.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/e820.h>
#include <drv/vga_textmode.h>
#include <lib/kprint.h>
#include <mm/pmm.h>
#include <sys/panic.h>

void kernel_init(void) {
    pic_8259_mask_all();
    pic_8259_remap(0x20, 0x28);

    load_gdt();
    init_idt();

    init_vga_textmode();

    kprint(KPRN_INFO, "Welcome to Phoenux");

    init_e820();

    go_to_ring1();
    // no real mode stuff after this

    init_pmm();

    asm volatile (
        "ud2"
    );
    panic(NULL, "Nothing to do");
}
