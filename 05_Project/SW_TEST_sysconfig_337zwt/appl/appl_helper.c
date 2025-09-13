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


