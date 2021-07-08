#include <sys/pic_8259.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <drv/com.h>
#include <lib/kprint.h>
#include <sys/panic.h>

void kernel_init(void) {
    com_init();

    kprint(KPRN_INFO, "Welcome to Phoenux");

    pic_8259_mask_all();
    pic_8259_remap(0x20, 0x28);

    load_gdt();
    init_idt();
    go_to_ring1();

    panic(NULL, true, "Nothing to do");
}
