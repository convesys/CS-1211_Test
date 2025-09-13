/*
 * appl_timer.c
 *
 *  Created on: 13.09.2025
 *      Author: Dimitar Lilov
 */

#include <stdint.h>
#include "board.h"
#include "mbprot.h"
#include "appl_debug.h"
#include "appl_timer.h"

uint16_t ui16_tmr0_cntr;

//
// Interrupt Functions
//

__interrupt void INT_CPUTIMER0_inst_ISR(void)
{
    MB_Check_Tmr();
    APPL_TIMER_200ms_Generation();
    Interrupt_clearACKGroup(INT_CPUTIMER0_inst_INTERRUPT_ACK_GROUP);
}

//
// Led Indication Timer Interrupt
//
void APPL_TIMER_200ms_Generation(void)
{
    if(20000 <= ++ui16_tmr0_cntr){
        ui16_tmr0_cntr = 0;
        APPL_DEBUG_ui16_snd_cntr++;
    } else {
        if(4000 >= ui16_tmr0_cntr){
//            GPIO_writePin(LED4_GPIO, 1);
            GPIO_writePin(LED3_GPIO, 1);
            GPIO_writePin(LED2_GPIO, 1);
//            GPIO_writePin(LED1_GPIO, 1);
        } else
        if(8000 >= ui16_tmr0_cntr){
//            GPIO_writePin(LED4_GPIO, 0);
            GPIO_writePin(LED3_GPIO, 1);
            GPIO_writePin(LED2_GPIO, 1);
//            GPIO_writePin(LED1_GPIO, 1);
        } else
        if(12000 >= ui16_tmr0_cntr){
//            GPIO_writePin(LED4_GPIO, 1);
            GPIO_writePin(LED3_GPIO, 0);
            GPIO_writePin(LED2_GPIO, 1);
//            GPIO_writePin(LED1_GPIO, 1);
        } else
        if(16000 >= ui16_tmr0_cntr){
//            GPIO_writePin(LED4_GPIO, 1);
            GPIO_writePin(LED3_GPIO, 1);
            GPIO_writePin(LED2_GPIO, 0);
//            GPIO_writePin(LED1_GPIO, 1);
        } else {
//            GPIO_writePin(LED4_GPIO, 1);
            GPIO_writePin(LED3_GPIO, 1);
            GPIO_writePin(LED2_GPIO, 1);
//            GPIO_writePin(LED1_GPIO, 0);
        }
    }
}

