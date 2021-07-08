#include <drv/com.h>
#include <lib/io.h>

void com_init(void) {
    outb(0x3f8 + 1, 0x00);
    outb(0x3f8 + 3, 0x80);
    outb(0x3f8 + 0, 0x01);
    outb(0x3f8 + 1, 0x00);
    outb(0x3f8 + 3, 0x03);
    outb(0x3f8 + 2, 0xc7);
    outb(0x3f8 + 4, 0x0b);
}

void com_out(uint8_t byte) {
    if (byte == '\n') {
        while ((inb(0x3f8 + 5) & 0x20) == 0) {}
        outb(0x3f8, '\r');
    }
    while ((inb(0x3f8 + 5) & 0x20) == 0) {}
    outb(0x3f8, byte);
}
