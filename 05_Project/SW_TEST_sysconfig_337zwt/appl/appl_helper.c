/*
 * appl_helper.c
 *
 *  Created on: 13.09.2025
 *      Author: Dimitar Lilov
 */

#include <stdint.h>

uint16_t APPL_HELPER_hex_str(uint16_t val)
{
    val &= 0x0000F;
    if(val > 9){
        val -= 10;
        val += 'A';
    } else val += '0';
    return val;
}

uint16_t APPL_HELPER_bcd2bin(uint16_t val)
{
    uint16_t res;
    res = ((val >> 12) & 0x000F) * 1000;
    res += ((val >> 8) & 0x000F) * 100;
    res += ((val >> 4) & 0x000F) * 10;
    res += (val & 0x000F);
    return res;
}

uint16_t APPL_HELPER_bin2bcd(uint16_t val)
{
    uint16_t res;
    res = ((val / 1000)%10) << 12;
    res += ((val / 100)%10) << 8;
    res += ((val / 10)%10) << 4;
    res += (val % 10);
    return res;
}


