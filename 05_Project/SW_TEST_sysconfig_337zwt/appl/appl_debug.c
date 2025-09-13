/*
 * appl_debug.c
 *
 *  Created on: 13.09.2025
 *      Author: Dimitar Lilov
 */

#include <stdint.h>
#include "board.h"
#include "appl_helper.h"

uint16_t APPL_DEBUG_ui16_snd_cntr;
uint16_t ui16_snd_cntrp;

void APPL_DEBUG_sci_test_print(void)
{
    if(APPL_DEBUG_ui16_snd_cntr != ui16_snd_cntrp){
        SCI_writeCharNonBlocking(SCIA_BASE, '0');
        SCI_writeCharNonBlocking(SCIA_BASE, 'x');
        SCI_writeCharNonBlocking(SCIA_BASE, APPL_HELPER_hex_str(APPL_DEBUG_ui16_snd_cntr >> 12));
        SCI_writeCharNonBlocking(SCIA_BASE, APPL_HELPER_hex_str(APPL_DEBUG_ui16_snd_cntr >> 8));
        SCI_writeCharNonBlocking(SCIA_BASE, APPL_HELPER_hex_str(APPL_DEBUG_ui16_snd_cntr >> 4));
        SCI_writeCharNonBlocking(SCIA_BASE, APPL_HELPER_hex_str(APPL_DEBUG_ui16_snd_cntr));
        SCI_writeCharNonBlocking(SCIA_BASE, '\r');
        SCI_writeCharNonBlocking(SCIA_BASE, '\n');
        ui16_snd_cntrp = APPL_DEBUG_ui16_snd_cntr;
    }
}
