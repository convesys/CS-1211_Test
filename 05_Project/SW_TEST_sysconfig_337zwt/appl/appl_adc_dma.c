/*
 * appl_adc_dma.c
 *
 *  Created on: 13.09.2025
 *      Author: Dimitar Lilov
 */

//
// Included Files
//
#include <stdint.h>
#include "board.h"
#include "appl_adc.h"


//
// Variables
//

uint16_t ui16_dma_cntr[4];

/* timer resolution set to 0.5 us */
uint16_t timer_final_dma[4];
uint16_t timer_count_dma[4];
uint16_t timer_count_adc_dma[4];        //take last adc counter - this is the most valid measured time for the requested series of adc conversions


//
// Interrupt Functions
//

//
// dmach1ISR - This is called at the end of the DMA transfer, the conversions
//              are stopped by removing the trigger of the first SOC from
//              the last.
//
__interrupt void INT_DMA1_inst_ISR(void)
{
    ui16_dma_cntr[0]++;
    //
    // Stop the ADC by removing the trigger for SOC0
    //
//    DMA_stopChannel(DMA1_inst_BASE);
//    DMA_disableTrigger(DMA1_inst_BASE);
//    DMA_clearTriggerFlag(DMA1_inst_BASE);
    APPL_ADC_StopTriggering(ADCA_BASE);
    //ADC_setInterruptSOCTrigger(ADCA_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);

    timer_final_dma[0] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    timer_count_dma[0] = APPL_ADC_timer_start_adc[0] - timer_final_dma[0];
    timer_count_adc_dma[0] = APPL_ADC_timer_count_adc[0];
    //APPL_ADC_MeasureStop();

    Interrupt_clearACKGroup(INT_DMA1_inst_INTERRUPT_ACK_GROUP);
}

__interrupt void INT_DMA2_inst_ISR(void)
{
    ui16_dma_cntr[1]++;
    //
    // Stop the ADC by removing the trigger for SOC0
    //
    APPL_ADC_StopTriggering(ADCB_BASE);
    //ADC_setInterruptSOCTrigger(ADCB_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);

    timer_final_dma[1] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    timer_count_dma[1] = APPL_ADC_timer_start_adc[1] - timer_final_dma[1];
    timer_count_adc_dma[1] = APPL_ADC_timer_count_adc[1];
    //APPL_ADC_MeasureStop();

    Interrupt_clearACKGroup(INT_DMA2_inst_INTERRUPT_ACK_GROUP);
}

__interrupt void INT_DMA3_inst_ISR(void)
{
    ui16_dma_cntr[2]++;
    //
    // Stop the ADC by removing the trigger for SOC0
    //
    APPL_ADC_StopTriggering(ADCC_BASE);
    //ADC_setInterruptSOCTrigger(ADCC_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);

    timer_final_dma[2] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    timer_count_dma[2] = APPL_ADC_timer_start_adc[2] - timer_final_dma[2];
    timer_count_adc_dma[2] = APPL_ADC_timer_count_adc[2];
    //APPL_ADC_MeasureStop();

    Interrupt_clearACKGroup(INT_DMA3_inst_INTERRUPT_ACK_GROUP);
}

__interrupt void INT_DMA4_inst_ISR(void)
{
    ui16_dma_cntr[3]++;
    //
    // Stop the ADC by removing the trigger for SOC0
    //
    APPL_ADC_StopTriggering(ADCD_BASE);
    //ADC_setInterruptSOCTrigger(ADCD_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);

    timer_final_dma[3] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    timer_count_dma[3] = APPL_ADC_timer_start_adc[3] - timer_final_dma[3];
    timer_count_adc_dma[3] = APPL_ADC_timer_count_adc[3];
    //APPL_ADC_MeasureStop();

    Interrupt_clearACKGroup(INT_DMA3_inst_INTERRUPT_ACK_GROUP);
}

