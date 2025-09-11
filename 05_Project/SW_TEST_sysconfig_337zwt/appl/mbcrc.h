#ifndef MBCRC_H
#define MBCRC_H

// Section: Included Files

#include <stdint.h>

#define MB_ADDRESS 0x01

#define DATA_OK 0
#define BAD_ADDR 0x02
#define BAD_CNT 0x03
#define DATA_NOK 0x04
#define ILLEGAL_CHECK_SUM 0xFF


void make_crc16 (uint16_t *buffer, uint16_t cnt);
uint16_t check_crc16 (uint16_t *buffer, uint16_t cnt);
#endif  // MBCRC_H
