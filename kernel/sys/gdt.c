#include <stdint.h>
#include <stddef.h>
#include <lib/symbol.h>
#include <sys/gdt.h>

extern symbol tss;
extern symbol tss_size;

typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_mid;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

static gdt_entry_t gdt[10];

typedef struct {
    uint16_t size;
    uint32_t base;
} __attribute__((packed)) gdt_ptr_t;

static gdt_ptr_t gdt_ptr = {
    (uint16_t)(sizeof(gdt) - 1),
    (uint32_t)gdt
};

void set_segment(int entry, size_t base, size_t page_count) {
    gdt[entry].base_low = (uint16_t)(base & 0x0000ffff);
    gdt[entry].base_mid = (uint8_t)((base & 0x00ff0000) / 0x10000);
    gdt[entry].base_high = (uint8_t)((base & 0xff000000) / 0x1000000);

    gdt[entry].limit_low = (uint16_t)((page_count - 1) & 0x0000ffff);
    gdt[entry].granularity = (uint8_t)((gdt[entry].granularity & 0b11110000) | ((page_count & 0x000f0000) / 0x10000));
}

void load_gdt(void) {
    // null pointer
    gdt[0].limit_low = 0;
    gdt[0].base_low = 0;
    gdt[0].base_mid = 0;
    gdt[0].access = 0;
    gdt[0].granularity = 0;
    gdt[0].base_high = 0;

    // define interrupt code
    gdt[1].limit_low = 0xffff;
    gdt[1].base_low = 0x0000;
    gdt[1].base_mid = 0x00;
    gdt[1].access = 0b10011010;
    gdt[1].granularity = 0b11001111;
    gdt[1].base_high = 0x00;

    // define interrupt data
    gdt[2].limit_low = 0xffff;
    gdt[2].base_low = 0x0000;
    gdt[2].base_mid = 0x00;
    gdt[2].access = 0b10010010;
    gdt[2].granularity = 0b11001111;
    gdt[2].base_high = 0x00;

    // define kernel (ring 1) code
    gdt[3].limit_low = 0xffff;
    gdt[3].base_low = 0x0000;
    gdt[3].base_mid = 0x00;
    gdt[3].access = 0b10111010;
    gdt[3].granularity = 0b11001111;
    gdt[3].base_high = 0x00;

    // define kernel (ring 1) data
    gdt[4].limit_low = 0xffff;
    gdt[4].base_low = 0x0000;
    gdt[4].base_mid = 0x00;
    gdt[4].access = 0b10110010;
    gdt[4].granularity = 0b11001111;
    gdt[4].base_high = 0x00;

    // define user code
    gdt[5].limit_low = 0x0000;
    gdt[5].base_low = 0x0000;
    gdt[5].base_mid = 0x00;
    gdt[5].access = 0b11111010;
    gdt[5].granularity = 0b11000000;
    gdt[5].base_high = 0x00;

    // define user data
    gdt[6].limit_low = 0x0000;
    gdt[6].base_low = 0x0000;
    gdt[6].base_mid = 0x00;
    gdt[6].access = 0b11110010;
    gdt[6].granularity = 0b11000000;
    gdt[6].base_high = 0x00;

    // define 16-bit code
    gdt[7].limit_low = 0xffff;
    gdt[7].base_low = 0x0000;
    gdt[7].base_mid = 0x00;
    gdt[7].access = 0b10011010;
    gdt[7].granularity = 0b10001111;
    gdt[7].base_high = 0x00;

    // define 16-bit data
    gdt[8].limit_low = 0xffff;
    gdt[8].base_low = 0x0000;
    gdt[8].base_mid = 0x00;
    gdt[8].access = 0b10010010;
    gdt[8].granularity = 0b10001111;
    gdt[8].base_high = 0x00;

    // define TSS segment
    gdt[9].limit_low = (uint16_t)((size_t)tss_size & 0x0000ffff);
    gdt[9].base_low = (uint16_t)((size_t)tss & 0x0000ffff);
    gdt[9].base_mid = (uint8_t)(((size_t)tss & 0x00ff0000) / 0x10000);
    gdt[9].access = 0b11101001;
    gdt[9].granularity = 0b00000000;
    gdt[9].base_high = (uint8_t)(((size_t)tss & 0xff000000) / 0x1000000);

    // effectively load the gdt
    asm volatile (
        "mov eax, %0;"
        "lgdt [eax];"
        "jmp 0x08:1f;"
        "1:"
        "mov ax, 0x10;"
        "mov ds, ax;"
        "mov es, ax;"
        "mov fs, ax;"
        "mov gs, ax;"
        "mov ss, ax;"
         :
         : "r" (&gdt_ptr)
         : "eax", "memory"
    );
}

void go_to_ring1(void) {
    asm volatile (
        "mov ax, 0x48;"
        "ltr ax;"
        "mov eax, esp;"
        "push 0x21;"
        "push eax;"
        "pushf;"
        "pop eax;"
        "or eax, 1 << 12;"
        "push eax;"
        "push 0x19;"
        "push 1f;"
        "iret;"
        "1: .long 2f;"
        "2:"
        "mov ax, 0x21;"
        "mov ds, ax;"
        "mov es, ax;"
        "mov fs, ax;"
        "mov gs, ax;"
         :
         :
         : "eax", "memory"
    );
}
