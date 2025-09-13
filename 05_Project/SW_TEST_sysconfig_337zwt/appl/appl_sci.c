/*
 * appl_sci.c
 *
 *  Created on: 13.09.2025
 *      Author: Dimitar Lilov
 */


//
// Include Files
//
#include <stdint.h>
#include "board.h"
#include "mbprot.h"

//
// Variables
//
uint16_t ui16_readval, ui16_readcnt;


//
// Interrupt Functions
//

__interrupt void INT_SCIA_inst_RX_ISR(void)
{
//    ui16_readval = SCI_readCharNonBlocking(SCIA_BASE);
    MBRXInterrupt();
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
    Interrupt_clearACKGroup(INT_SCIA_inst_RX_INTERRUPT_ACK_GROUP);
    ui16_readcnt++;
}

__interrupt void INT_SCIA_inst_TX_ISR(void)
{
    MBTXInterrupt();
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
    Interrupt_clearACKGroup(INT_SCIA_inst_TX_INTERRUPT_ACK_GROUP);
//    Interrupt_disable(INT_SCIA_inst_TX);
}

