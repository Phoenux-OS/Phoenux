#ifndef __LIB__IO_H__
#define __LIB__IO_H__

#include <stdint.h>

void outb(uint16_t port, uint8_t value);
#pragma aux outb =      \
    "out dx, al"        \
    parm [ dx ] [ al ];

void outw(uint16_t port, uint16_t value);
#pragma aux outw =      \
    "out dx, ax"        \
    parm [ dx ] [ ax ];

void outd(uint16_t port, uint32_t value);
#pragma aux outd =       \
    "out dx, eax"        \
    parm [ dx ] [ eax ];

uint8_t inb(uint16_t port);
#pragma aux inb = \
    "in al, dx"   \
    parm [ dx ]   \
    value [ al ];

uint16_t inw(uint16_t port);
#pragma aux inw = \
    "in ax, dx"   \
    parm [ dx ]   \
    value [ ax ];

uint32_t ind(uint16_t port);
#pragma aux ind = \
    "in eax, dx"  \
    parm [ dx ]   \
    value [ eax ];

#define io_wait() (outb(0x80, 0x00))

#endif
