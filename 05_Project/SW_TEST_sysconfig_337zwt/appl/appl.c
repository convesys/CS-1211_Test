/*
 * appl.c
 *
 *  Created on: 1 Sep 2025
 *      Author: Deyan Todorov
 */

//
// Included Files
//
#include <stdint.h>
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "mbprot.h"
#include "mbdata.h"

#include "appl_gpio.h"
#include "appl_adc.h"
#include "appl_adc_dma.h"
#include "appl_timer.h"
#include "appl_debug.h"
#include "appl_sci.h"

//
// Application Functions
//

void APPL_main(void)
{
    uint16_t timer_start = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    //APPL_ADC_Start();
    APPL_GPIO_Fill();
    APPL_ADC_Fill();
    uint16_t timer_final = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    inp_reg_mem[100] =  timer_start - timer_final;
    MB_Main();

    APPL_ADC_process();
}

//
// Interrupt Functions
//

void APPL_preinit(void)
{
    Interrupt_initModule();
    Interrupt_initVectorTable();
}

void APPL_postinit(void)
{
    APPL_ADC_init();
    EINT;
    ERTM;
}

//
// End of File
//


