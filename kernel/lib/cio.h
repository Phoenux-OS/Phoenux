#ifndef __LIB__CIO_H__
#define __LIB__CIO_H__

#include <stdint.h>

void port_out_b(uint16_t port, uint8_t  value);
void port_out_w(uint16_t port, uint16_t value);
void port_out_d(uint16_t port, uint32_t value);

uint8_t  port_in_b(uint16_t port);
uint16_t port_in_w(uint16_t port);
uint32_t port_in_d(uint16_t port);

#define io_wait() (port_out_b(0x80, 0x00))

#endif
