/*
 * appl_adc.c
 *
 *  Created on: 13.09.2025
 *      Author: Dimitar Lilov
 */

//
// Included Files
//

#include <stdint.h>
#include "appl_adc.h"
#include "board.h"

//
// Variables
//

#pragma DATA_SECTION(ADCA_ResultBuffer, "ramgs0_15");
#pragma DATA_SECTION(ADCB_ResultBuffer, "ramgs0_15");
#pragma DATA_SECTION(ADCC_ResultBuffer, "ramgs0_15");
#pragma DATA_SECTION(ADCD_ResultBuffer, "ramgs0_15");
uint16_t ADCA_ResultBuffer[APPL_ADC_RESULTS_BUFFER_SIZE + 16];  //value index 0 could be stale read by dma when ADC prescale Ratio > 2 - take data starting from index 1..16 - read 16 more values to compensate
uint16_t ADCB_ResultBuffer[APPL_ADC_RESULTS_BUFFER_SIZE + 16];  //value index 0 could be stale read by dma when ADC prescale Ratio > 2 - take data starting from index 1..16 - read 16 more values to compensate
uint16_t ADCC_ResultBuffer[APPL_ADC_RESULTS_BUFFER_SIZE + 16];  //value index 0 could be stale read by dma when ADC prescale Ratio > 2 - take data starting from index 1..16 - read 16 more values to compensate
uint16_t ADCD_ResultBuffer[APPL_ADC_RESULTS_BUFFER_SIZE + 16];  //value index 0 could be stale read by dma when ADC prescale Ratio > 2 - take data starting from index 1..16 - read 16 more values to compensate

const void * ADCA_ResultBufferPtr = ADCA_ResultBuffer;
const void * ADCB_ResultBufferPtr = ADCB_ResultBuffer;
const void * ADCC_ResultBufferPtr = ADCC_ResultBuffer;
const void * ADCD_ResultBufferPtr = ADCD_ResultBuffer;

const void * ADCA_ResultRegisterPtr = (const void *)ADCARESULT_BASE;
const void * ADCB_ResultRegisterPtr = (const void *)ADCBRESULT_BASE;
const void * ADCC_ResultRegisterPtr = (const void *)ADCCRESULT_BASE;
const void * ADCD_ResultRegisterPtr = (const void *)ADCDRESULT_BASE;



uint16_t bManualMeasureConfigure = 1;
uint16_t bManualMeasureRequest = 0;
uint16_t bManualMeasureStop = 0;

uint16_t ADCA_InterruptCount[4];
uint16_t ADCB_InterruptCount[4];
uint16_t ADCC_InterruptCount[4];
uint16_t ADCD_InterruptCount[4];

/* timer resolution set to 0.5 us */
uint16_t APPL_ADC_timer_start_adc[4];
uint16_t timer_final_adc[4];
uint16_t APPL_ADC_timer_count_adc[4];


//
// Interrupt Functions
//

__interrupt void INT_ADCA_inst_1_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCA1);
    ADCA_InterruptCount[0]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

__interrupt void INT_ADCA_inst_2_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCA2);
    ADCA_InterruptCount[1]++;

    timer_final_adc[0] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    APPL_ADC_timer_count_adc[0] = APPL_ADC_timer_start_adc[0] - timer_final_adc[0];

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCA_inst_3_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCA3);
    ADCA_InterruptCount[2]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCA_inst_4_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCA4);
    ADCA_InterruptCount[3]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCB_inst_1_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCB1);
    ADCB_InterruptCount[0]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

__interrupt void INT_ADCB_inst_2_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCB2);
    ADCB_InterruptCount[1]++;

    timer_final_adc[1] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    APPL_ADC_timer_count_adc[1] = APPL_ADC_timer_start_adc[1] - timer_final_adc[1];

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCB_inst_3_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCB3);
    ADCB_InterruptCount[2]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCB_inst_4_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCB4);
    ADCB_InterruptCount[3]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCC_inst_1_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCC1);
    ADCC_InterruptCount[0]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

__interrupt void INT_ADCC_inst_2_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCC2);
    ADCC_InterruptCount[1]++;

    timer_final_adc[2] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    APPL_ADC_timer_count_adc[2] = APPL_ADC_timer_start_adc[2] - timer_final_adc[2];

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCC_inst_3_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCC3);
    ADCC_InterruptCount[2]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCC_inst_4_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCC4);
    ADCC_InterruptCount[3]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCD_inst_1_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCD1);
    ADCD_InterruptCount[0]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

__interrupt void INT_ADCD_inst_2_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCD2);
    ADCD_InterruptCount[1]++;

    timer_final_adc[3] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    APPL_ADC_timer_count_adc[3] = APPL_ADC_timer_start_adc[3] - timer_final_adc[3];

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCD_inst_3_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCD3);
    ADCD_InterruptCount[2]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

__interrupt void INT_ADCD_inst_4_ISR(void)
{
    //
    // Disable this interrupt from happening again
    //
    //Interrupt_disable(INT_ADCD4);
    ADCD_InterruptCount[3]++;

    //
    // Acknowledge interrupt
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP10);
}

//
// setupADCContinuous - setup the ADC to continuously convert on one channel
//
void setupADCContinuous(uint32_t adcBase, uint16_t channel)
{
    uint16_t acqps;

    //
    // Determine minimum acquisition window (in SYSCLKS) based on resolution
    //
    if(APPL_ADC_RESOLUTION == 12)
    {
        acqps = 14; // 75ns sampling time
                    // 44 cycles conversion time
    }
    else //resolution is 16-bit
    {
        acqps = 63; // 320ns
    }
    //
    // - NOTE: A longer sampling window will be required if the ADC driving
    //   source is less than ideal (an ideal source would be a high bandwidth
    //   op-amp with a small series resistance). See TI application report
    //   SPRACT6 for guidance on ADC driver design.
    //

    //
    // Configure SOCs channel no. & acquisition window.
    // Trigger SCO0 from SW.
    // Trigger all other SOCs from INT1 (EOC on SOC0).
    //
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER1, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER2, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER3, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER4, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER5, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER6, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER7, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER8, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER9, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER10, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER11, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER12, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER13, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER14, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);
    ADC_setupSOC(adcBase, ADC_SOC_NUMBER15, ADC_TRIGGER_SW_ONLY,
                 (ADC_Channel)channel, acqps);

    //
    // Enable continuous operation by setting the last SOC to re-trigger
    // the first
    //
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER0,    // ADCA
                               ADC_INT_SOC_TRIGGER_ADCINT2);
    //
    // Configure ADCINT1 trigger for SOC1-SOC15
    //
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER1,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER2,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER3,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER4,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER5,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER6,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER7,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER8,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER9,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER10,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER11,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER12,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER13,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER14,
                               ADC_INT_SOC_TRIGGER_ADCINT1);
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER15,
                               ADC_INT_SOC_TRIGGER_ADCINT1);

    //
    // Enable ADCINT1 & ADCINT2. Disable ADCINT3 & ADCINT4.
    //
    ADC_enableInterrupt(adcBase, ADC_INT_NUMBER1);
    ADC_enableInterrupt(adcBase, ADC_INT_NUMBER2);
    ADC_disableInterrupt(adcBase, ADC_INT_NUMBER3);
    ADC_disableInterrupt(adcBase, ADC_INT_NUMBER4);

    //
    // Enable continuous mode
    //
    ADC_enableContinuousMode(adcBase, ADC_INT_NUMBER1);
    ADC_enableContinuousMode(adcBase, ADC_INT_NUMBER2);

    //
    // Configure interrupt triggers
    //
    ADC_setInterruptSource(adcBase, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_setInterruptSource(adcBase, ADC_INT_NUMBER2, ADC_SOC_NUMBER15);

}





void APPL_ADC_process(void)
{
    if (bManualMeasureConfigure)
    {
        setupADCContinuous(ADCA_BASE, ADC_CH_ADCIN0);
        bManualMeasureConfigure = 0;
    }
    if (bManualMeasureRequest)
    {
        //re-enable trigger ADC_SOC_NUMBER0 with ADCINT2 for continuous adc mode
        ADC_setInterruptSOCTrigger(ADCA_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_ADCINT2);
        ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
        APPL_ADC_timer_start_adc[0] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
        bManualMeasureRequest = 0;
    }
    if (bManualMeasureStop)
    {
        ADC_setInterruptSOCTrigger(ADCA_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);
        bManualMeasureStop = 0;
    }
}







//
//obsolete tryouts
//
#if 0

void APPL_ADC_Start(void)
{
    ADC_forceMultipleSOC(ADCA_BASE, ADC_FORCE_SOC0 | ADC_FORCE_SOC1 | ADC_FORCE_SOC2 | ADC_FORCE_SOC3 | ADC_FORCE_SOC4 | ADC_FORCE_SOC5);
    ADC_forceMultipleSOC(ADCB_BASE, ADC_FORCE_SOC0 | ADC_FORCE_SOC1 | ADC_FORCE_SOC2 | ADC_FORCE_SOC3 | ADC_FORCE_SOC4 | ADC_FORCE_SOC5);
    ADC_forceMultipleSOC(ADCC_BASE, ADC_FORCE_SOC0 | ADC_FORCE_SOC1 | ADC_FORCE_SOC2 | ADC_FORCE_SOC3 | ADC_FORCE_SOC4 | ADC_FORCE_SOC5);
    ADC_forceMultipleSOC(ADCD_BASE, ADC_FORCE_SOC0 | ADC_FORCE_SOC1 | ADC_FORCE_SOC2 | ADC_FORCE_SOC3 | ADC_FORCE_SOC4 | ADC_FORCE_SOC5);
}

void APPL_ADC_Fill(void)
{

//    while(ADC_getInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1) == false)
//    {
//    }
//    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
//
//    while(ADC_getInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1) == false)
//    {
//    }
//    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);
//
//    while(ADC_getInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1) == false)
//    {
//    }
//    ADC_clearInterruptStatus(ADCC_BASE, ADC_INT_NUMBER1);
//
//    while(ADC_getInterruptStatus(ADCD_BASE, ADC_INT_NUMBER1) == false)
//    {
//    }
//    ADC_clearInterruptStatus(ADCD_BASE, ADC_INT_NUMBER1);

    inp_reg_mem[32] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);
    inp_reg_mem[33] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER1);
    inp_reg_mem[34] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER2);
    inp_reg_mem[35] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER3);
    inp_reg_mem[36] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER4);
    inp_reg_mem[37] = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER5);

    inp_reg_mem[38] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);
    inp_reg_mem[39] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER1);
    inp_reg_mem[40] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER2);
    inp_reg_mem[41] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER3);
    inp_reg_mem[42] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER4);
    inp_reg_mem[43] = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER5);

    inp_reg_mem[44] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER0);
    inp_reg_mem[45] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER1);
    inp_reg_mem[46] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER2);
    inp_reg_mem[47] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER3);
    inp_reg_mem[48] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER4);
    inp_reg_mem[49] = ADC_readResult(ADCCRESULT_BASE, ADC_SOC_NUMBER5);

    inp_reg_mem[50] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER0);
    inp_reg_mem[51] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER1);
    inp_reg_mem[52] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER2);
    inp_reg_mem[53] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER3);
    inp_reg_mem[54] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER4);
    inp_reg_mem[55] = ADC_readResult(ADCDRESULT_BASE, ADC_SOC_NUMBER5);
}


void APPL_ADC_MeasureStart(void)
{
    setupADCContinuous(ADCA_BASE, ADC_CH_ADCIN0);
    APPL_DMA_ADCA();
    ADC_forceMultipleSOC(ADCA_BASE, ADC_FORCE_SOC0);
}

void APPL_ADC_MeasureStop(void)
{
    DMA_stopChannel(DMA_CH1_BASE);
    DMA_disableTrigger(DMA_CH1_BASE);
    DMA_disableOverrunInterrupt(DMA_CH1_BASE);
    DMA_disableInterrupt(DMA_CH1_BASE);

    //
    // Enable continuous mode
    //
    ADC_disableContinuousMode(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_disableContinuousMode(ADCA_BASE, ADC_INT_NUMBER2);

    //
    // Enable ADCINT1 & ADCINT2. Disable ADCINT3 & ADCINT4.
    //
    ADC_disableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_disableInterrupt(ADCA_BASE, ADC_INT_NUMBER2);
    ADC_disableInterrupt(ADCA_BASE, ADC_INT_NUMBER3);
    ADC_disableInterrupt(ADCA_BASE, ADC_INT_NUMBER4);
}

void APPL_DMA_ADCA(void)
{
    //
    // Clearing all pending interrupt flags
    //
    DMA_clearTriggerFlag(DMA_CH1_BASE);   // DMA channel 1
    HWREGH(ADCA_BASE + ADC_O_INTFLGCLR) = 0x3U; // ADCA

    //
    // DMA channel 1 set up for ADCA
    //
    DMA_configAddresses(DMA_CH1_BASE, ADCA_ResultBufferPtr, ADCA_ResultRegisterPtr);

    //
    // Perform enough 16-word bursts to fill the results buffer. Data will be
    // transferred 32 bits at a time hence the address steps below.
    //
    DMA_configBurst(DMA_CH1_BASE, 16, 2, 2);
    DMA_configTransfer(DMA_CH1_BASE, (APPL_ADC_RESULTS_BUFFER_SIZE >> 4), -14, 2);
    DMA_configMode(DMA_CH1_BASE, DMA_TRIGGER_ADCA2,
                   (DMA_CFG_ONESHOT_DISABLE | DMA_CFG_CONTINUOUS_DISABLE |
                    DMA_CFG_SIZE_32BIT));

    DMA_enableTrigger(DMA_CH1_BASE);
    DMA_disableOverrunInterrupt(DMA_CH1_BASE);
    DMA_setInterruptMode(DMA_CH1_BASE, DMA_INT_AT_END);
    DMA_enableInterrupt(DMA_CH1_BASE);
}

#endif
