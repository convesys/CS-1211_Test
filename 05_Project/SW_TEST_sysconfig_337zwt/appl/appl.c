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
#include "appl_spi.h"
#include "appl_rtc.h"

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
    APPL_SPI_process();
    APPL_RTC_process();
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
    EALLOW;
    // GPIO72 -> inst_GPIO72 Pinmux
    GPIO_setPinConfig(GPIO_72_GPIO72);
    // GPIO61 -> inst_GPIO61 Pinmux
    GPIO_setPinConfig(GPIO_61_GPIO61);
    inst_GPIO72_init();
    inst_GPIO61_init();

    EDIS;

    APPL_ADC_init();
    EINT;
    ERTM;
    I2C_CLKS_SEC = 00;
    I2C_CLKS_MIN = 50;
    I2C_CLKS_HOUR = 19;
    I2C_CLKS_WDAY = 7;
    I2C_CLKS_DAY = 21;
    I2C_CLKS_MNT = 9;
    I2C_CLKS_YEAR = 25;
}

//
// End of File
//


