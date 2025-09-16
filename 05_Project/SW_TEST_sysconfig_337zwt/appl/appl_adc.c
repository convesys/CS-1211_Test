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
#include "board.h"
#include "mbdata.h"
#include "appl_adc.h"

//
// Configuration Definitions
//


//
// Typedefs
//

typedef enum
{
    ADC_STATE_STOPPED,
    ADC_STATE_RUNNING

} eADCState_t;

//
// Variables
//

#pragma DATA_SECTION(ADCA_ResultBuffer, "ramgs0_15");
#pragma DATA_SECTION(ADCB_ResultBuffer, "ramgs0_15");
#pragma DATA_SECTION(ADCC_ResultBuffer, "ramgs0_15");
#pragma DATA_SECTION(ADCD_ResultBuffer, "ramgs0_15");
uint16_t ADCA_ResultBuffer[APPL_ADC_RESULTS_BUFFER_SIZE];  //value index 0 could be stale read by dma when ADC prescale Ratio > 2 - take data starting from index 1..16 - read 16 more values to compensate
uint16_t ADCB_ResultBuffer[APPL_ADC_RESULTS_BUFFER_SIZE];  //value index 0 could be stale read by dma when ADC prescale Ratio > 2 - take data starting from index 1..16 - read 16 more values to compensate
uint16_t ADCC_ResultBuffer[APPL_ADC_RESULTS_BUFFER_SIZE];  //value index 0 could be stale read by dma when ADC prescale Ratio > 2 - take data starting from index 1..16 - read 16 more values to compensate
uint16_t ADCD_ResultBuffer[APPL_ADC_RESULTS_BUFFER_SIZE];  //value index 0 could be stale read by dma when ADC prescale Ratio > 2 - take data starting from index 1..16 - read 16 more values to compensate

uint16_t ADCA_Results[16];
uint16_t ADCB_Results[16];
uint16_t ADCC_Results[16];
uint16_t ADCD_Results[16];

const void * ADCA_ResultBufferPtr = ADCA_ResultBuffer;
const void * ADCB_ResultBufferPtr = ADCB_ResultBuffer;
const void * ADCC_ResultBufferPtr = ADCC_ResultBuffer;
const void * ADCD_ResultBufferPtr = ADCD_ResultBuffer;

const void * ADCA_ResultRegisterPtr = (const void *)ADCARESULT_BASE;
const void * ADCB_ResultRegisterPtr = (const void *)ADCBRESULT_BASE;
const void * ADCC_ResultRegisterPtr = (const void *)ADCCRESULT_BASE;
const void * ADCD_ResultRegisterPtr = (const void *)ADCDRESULT_BASE;

uint16_t ADC_ChannelsCount = 6;
uint16_t ADC_Channel_Index = 0;
eADCState_t ADC_Auto_Measure_State = ADC_STATE_STOPPED;
uint16_t ADC_Auto_Measure_Request = 0;
uint16_t ADC_Auto_Measure_Request_Single_Mode = 0;
uint16_t ADC_Auto_Measure_Completed = 0;

uint16_t ADC_Auto_Measure_Use_Index_Fixed = 0;
uint16_t ADC_Channel_Index_Fixed = ADC_SOC_NUMBER0;     /* Corresponds to ADCA_Channels[ADC_Channel_Index_Fixed] to be measured */

uint16_t ADC_Measurement_InProgress[4];
uint16_t ADC_Measurement_WaitComplete[4];

uint16_t ADCA_Channels[16] =
{
 ADC_CH_ADCIN0,
 ADC_CH_ADCIN1,
 ADC_CH_ADCIN2,
 ADC_CH_ADCIN3,
 ADC_CH_ADCIN4,
 ADC_CH_ADCIN5,
};

uint16_t ADCB_Channels[16] =
{
 ADC_CH_ADCIN0,
 ADC_CH_ADCIN1,
 ADC_CH_ADCIN2,
 ADC_CH_ADCIN3,
 ADC_CH_ADCIN4,
 ADC_CH_ADCIN5,
};

uint16_t ADCC_Channels[16] =
{
 ADC_CH_ADCIN14,
 ADC_CH_ADCIN15,
 ADC_CH_ADCIN2,
 ADC_CH_ADCIN3,
 ADC_CH_ADCIN4,
 ADC_CH_ADCIN5,
};

uint16_t ADCD_Channels[16] =
{
 ADC_CH_ADCIN0,
 ADC_CH_ADCIN1,
 ADC_CH_ADCIN2,
 ADC_CH_ADCIN3,
 ADC_CH_ADCIN4,
 ADC_CH_ADCIN5,
};


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



uint16_t DMA_StartDelayLoops[4];

uint16_t DMA_StartDelayLoopsConfig = APPL_ADC_START_DMA_DELAYED_ADC_CYCLES;


//
// Function Prototypes
//

void APPL_ADC_StopMeasurementDetect(uint16_t adcIndex);
void APPL_ADC_DelayedDMAStart(uint16_t adcIndex);
void APPL_ADC_SineWaveEvaluate (uint16_t adcIndex, uint16_t adcSOC);

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
    APPL_ADC_StopMeasurementDetect(0);
    APPL_ADC_DelayedDMAStart(0);

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
    APPL_ADC_StopMeasurementDetect(1);
    APPL_ADC_DelayedDMAStart(1);

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
    APPL_ADC_StopMeasurementDetect(2);
    APPL_ADC_DelayedDMAStart(2);

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
    APPL_ADC_StopMeasurementDetect(3);
    APPL_ADC_DelayedDMAStart(3);

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

    ADC_disableInterrupt(adcBase, ADC_INT_NUMBER1);
    ADC_disableInterrupt(adcBase, ADC_INT_NUMBER2);
    //
    // Determine minimum acquisition window (in SYSCLKS) based on resolution
    //
    if(APPL_ADC_RESOLUTION == 12)
    {
        acqps = 14; // 75ns sampling time (SYSCLK Ticks + 1 - at 200MHz 14 means (14+1)*5ns->75 ns)
        acqps = 19; // 100ns sampling time (SYSCLK Ticks + 1 - at 200MHz 19 means (19+1)*5ns->100 ns)
                    // Table 11-12. ADC Timings in 12-bit Mode 44 cycles conversion time - 44 cycles @ 200MHz (adc presacaler 1:4) -> 44*5ns -> 220ns
        acqps = APPL_ADC_CHANNEL_SAMPLING_SYSCLK_TICKS - 1;
        //acqps = 63; // 320ns
        //acqps = 127; // 640ns
        //acqps = 255; // 1280ns
        //acqps = 511; // 2560ns
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
    // Enable continuous operation by setting the last SOC to re-trigger
    // the first
    //
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER0,    // ADCA
                               ADC_INT_SOC_TRIGGER_ADCINT2);
    //
    // Enable ADCINT1 & ADCINT2. Disable ADCINT3 & ADCINT4.
    //

    ADC_clearInterruptStatus(adcBase, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(adcBase, ADC_INT_NUMBER2);
    ADC_clearInterruptStatus(adcBase, ADC_INT_NUMBER3);
    ADC_clearInterruptStatus(adcBase, ADC_INT_NUMBER4);

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

void APPL_ADC_init(void)
{
    ADC_Auto_Measure_State = ADC_STATE_STOPPED;
    ADC_Channel_Index = 0;
    ADC_Auto_Measure_Request = 1;
}

void APPL_ADC_process(void)
{
    switch (ADC_Auto_Measure_State)
    {
    case ADC_STATE_STOPPED:
    {
        if (ADC_Auto_Measure_Request)
        {
            if (ADC_Auto_Measure_Request_Single_Mode)
            {
                ADC_Auto_Measure_Request = 0;
            }

            if (ADC_Auto_Measure_Use_Index_Fixed)
            {
                ADC_Channel_Index = ADC_Channel_Index_Fixed;
            }
            else
            {
                ADC_Channel_Index = 0;
            }

            setupADCContinuous(ADCA_BASE, ADCA_Channels[ADC_Channel_Index]);
            setupADCContinuous(ADCB_BASE, ADCB_Channels[ADC_Channel_Index]);
            setupADCContinuous(ADCC_BASE, ADCC_Channels[ADC_Channel_Index]);
            setupADCContinuous(ADCD_BASE, ADCD_Channels[ADC_Channel_Index]);
            ADC_Auto_Measure_State = ADC_STATE_RUNNING;
            ADC_Measurement_WaitComplete[0] = 0;
            ADC_Measurement_WaitComplete[1] = 0;
            ADC_Measurement_WaitComplete[2] = 0;
            ADC_Measurement_WaitComplete[3] = 0;
            ADC_Measurement_InProgress[0] = 1;
            ADC_Measurement_InProgress[1] = 1;
            ADC_Measurement_InProgress[2] = 1;
            ADC_Measurement_InProgress[3] = 1;
            ADC_Auto_Measure_Completed = 0;

            DMA_StartDelayLoops[0] = DMA_StartDelayLoopsConfig;
            DMA_StartDelayLoops[1] = DMA_StartDelayLoopsConfig;
            DMA_StartDelayLoops[2] = DMA_StartDelayLoopsConfig;
            DMA_StartDelayLoops[3] = DMA_StartDelayLoopsConfig;
            if (DMA_StartDelayLoops[0] == 0)
            {
                DMA_clearTriggerFlag(DMA1_inst_BASE);   //clear previous ADC triggers
                DMA_clearTriggerFlag(DMA2_inst_BASE);   //clear previous ADC triggers
                DMA_clearTriggerFlag(DMA3_inst_BASE);   //clear previous ADC triggers
                DMA_clearTriggerFlag(DMA4_inst_BASE);   //clear previous ADC triggers
//                DMA_enableTrigger(DMA1_inst_BASE);
//                DMA_enableTrigger(DMA2_inst_BASE);
//                DMA_enableTrigger(DMA3_inst_BASE);
//                DMA_enableTrigger(DMA4_inst_BASE);
                DMA_startChannel(DMA1_inst_BASE);
                DMA_startChannel(DMA2_inst_BASE);
                DMA_startChannel(DMA3_inst_BASE);
                DMA_startChannel(DMA4_inst_BASE);
            }

            APPL_ADC_timer_start_adc[0] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
            APPL_ADC_timer_start_adc[1] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
            APPL_ADC_timer_start_adc[2] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
            APPL_ADC_timer_start_adc[3] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
            ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
            ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER0);
            ADC_forceSOC(ADCC_BASE, ADC_SOC_NUMBER0);
            ADC_forceSOC(ADCD_BASE, ADC_SOC_NUMBER0);
        }
        break;
    }
    case ADC_STATE_RUNNING:
    {
        if (   (ADC_Measurement_InProgress[0] == 0)
            && (ADC_Measurement_InProgress[1] == 0)
            && (ADC_Measurement_InProgress[2] == 0)
            && (ADC_Measurement_InProgress[3] == 0) )
        {
            ADC_Auto_Measure_Completed = 1;

            APPL_ADC_SineWaveEvaluate (0, ADC_Channel_Index);
            APPL_ADC_SineWaveEvaluate (1, ADC_Channel_Index);
            APPL_ADC_SineWaveEvaluate (2, ADC_Channel_Index);
            APPL_ADC_SineWaveEvaluate (3, ADC_Channel_Index);

            //take results - for now only the first valid measurement result
//            ADCA_Results[ADC_Channel_Index] = ADCA_ResultBuffer[1];
//            ADCB_Results[ADC_Channel_Index] = ADCB_ResultBuffer[1];
//            ADCC_Results[ADC_Channel_Index] = ADCC_ResultBuffer[1];
//            ADCD_Results[ADC_Channel_Index] = ADCD_ResultBuffer[1];
       }

        if (ADC_Auto_Measure_Completed)
        {
            if (ADC_Auto_Measure_Request)
            {
                if (ADC_Auto_Measure_Request_Single_Mode)
                {
                    ADC_Auto_Measure_Request = 0;
                }

                if (ADC_Auto_Measure_Use_Index_Fixed)
                {
                    ADC_Channel_Index = ADC_Channel_Index_Fixed;
                }
                else
                {
                    ADC_Channel_Index++;
                    if (ADC_Channel_Index >= ADC_ChannelsCount)
                    {
                        ADC_Channel_Index = 0;
                    }
                }
                setupADCContinuous(ADCA_BASE, ADCA_Channels[ADC_Channel_Index]);
                setupADCContinuous(ADCB_BASE, ADCB_Channels[ADC_Channel_Index]);
                setupADCContinuous(ADCC_BASE, ADCC_Channels[ADC_Channel_Index]);
                setupADCContinuous(ADCD_BASE, ADCD_Channels[ADC_Channel_Index]);

                //re-enable trigger ADC_SOC_NUMBER0 with ADCINT2 for continuous adc mode if not used setupADCContinuous
    //            ADC_setInterruptSOCTrigger(ADCA_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_ADCINT2);
    //            ADC_setInterruptSOCTrigger(ADCB_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_ADCINT2);
    //            ADC_setInterruptSOCTrigger(ADCC_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_ADCINT2);
    //            ADC_setInterruptSOCTrigger(ADCD_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_ADCINT2);

                //ADC_Auto_Measure_State = ADC_STATE_RUNNING;
                ADC_Measurement_WaitComplete[0] = 0;
                ADC_Measurement_WaitComplete[1] = 0;
                ADC_Measurement_WaitComplete[2] = 0;
                ADC_Measurement_WaitComplete[3] = 0;
                ADC_Measurement_InProgress[0] = 1;
                ADC_Measurement_InProgress[1] = 1;
                ADC_Measurement_InProgress[2] = 1;
                ADC_Measurement_InProgress[3] = 1;
                ADC_Auto_Measure_Completed = 0;


                DMA_StartDelayLoops[0] = DMA_StartDelayLoopsConfig;
                DMA_StartDelayLoops[1] = DMA_StartDelayLoopsConfig;
                DMA_StartDelayLoops[2] = DMA_StartDelayLoopsConfig;
                DMA_StartDelayLoops[3] = DMA_StartDelayLoopsConfig;
                if (DMA_StartDelayLoops[0] == 0)
                {
                    DMA_clearTriggerFlag(DMA1_inst_BASE);   //clear previous ADC triggers
                    DMA_clearTriggerFlag(DMA2_inst_BASE);   //clear previous ADC triggers
                    DMA_clearTriggerFlag(DMA3_inst_BASE);   //clear previous ADC triggers
                    DMA_clearTriggerFlag(DMA4_inst_BASE);   //clear previous ADC triggers
    //                DMA_enableTrigger(DMA1_inst_BASE);
    //                DMA_enableTrigger(DMA2_inst_BASE);
    //                DMA_enableTrigger(DMA3_inst_BASE);
    //                DMA_enableTrigger(DMA4_inst_BASE);
                    DMA_startChannel(DMA1_inst_BASE);
                    DMA_startChannel(DMA2_inst_BASE);
                    DMA_startChannel(DMA3_inst_BASE);
                    DMA_startChannel(DMA4_inst_BASE);
                }



                APPL_ADC_timer_start_adc[0] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
                APPL_ADC_timer_start_adc[1] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
                APPL_ADC_timer_start_adc[2] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
                APPL_ADC_timer_start_adc[3] = CPUTimer_getTimerCount(CPUTIMER1_BASE);
                ADC_forceSOC(ADCA_BASE, ADC_SOC_NUMBER0);
                ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER0);
                ADC_forceSOC(ADCC_BASE, ADC_SOC_NUMBER0);
                ADC_forceSOC(ADCD_BASE, ADC_SOC_NUMBER0);
            }
            else
            {
                ADC_Auto_Measure_State = ADC_STATE_STOPPED;
            }
        }

        break;
    }
    }

}

void APPL_ADC_DelayedDMAStart(uint16_t adcIndex)
{
    if (DMA_StartDelayLoops[adcIndex])
    {
        DMA_StartDelayLoops[adcIndex]--;
        if (DMA_StartDelayLoops[adcIndex] == 0)
        {
            switch(adcIndex)
            {
            case 0:
                DMA_clearTriggerFlag(DMA1_inst_BASE);
                DMA_startChannel(DMA1_inst_BASE);
                break;
            case 1:
                DMA_clearTriggerFlag(DMA2_inst_BASE);
                DMA_startChannel(DMA2_inst_BASE);
                break;
            case 2:
                DMA_clearTriggerFlag(DMA3_inst_BASE);
                DMA_startChannel(DMA3_inst_BASE);
                break;
            case 3:
                DMA_clearTriggerFlag(DMA4_inst_BASE);
                DMA_startChannel(DMA4_inst_BASE);
                break;
            }
        }
    }
}


void APPL_ADC_StopTriggering(uint32_t adcBase)
{
    ADC_setInterruptSOCTrigger(adcBase, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);
    switch(adcBase)
    {
    case ADCA_BASE:
    {
        ADC_Measurement_WaitComplete[0] = 1;
    }
    case ADCB_BASE:
    {
        ADC_Measurement_WaitComplete[1] = 1;
    }
    case ADCC_BASE:
    {
        ADC_Measurement_WaitComplete[2] = 1;
    }
    case ADCD_BASE:
    {
        ADC_Measurement_WaitComplete[3] = 1;
    }
    }
}

void APPL_ADC_StopMeasurementDetect(uint16_t adcIndex)
{
    if (ADC_Measurement_InProgress[adcIndex])
    {
        if (ADC_Measurement_WaitComplete[adcIndex])
        {
            ADC_Measurement_WaitComplete[adcIndex] = 0;
            ADC_Measurement_InProgress[adcIndex] = 0;
        }
    }
}


void APPL_ADC_Fill(void)
{
    inp_reg_mem[32] = ADCA_Results[ADC_SOC_NUMBER0];
    inp_reg_mem[33] = ADCA_Results[ADC_SOC_NUMBER1];
    inp_reg_mem[34] = ADCA_Results[ADC_SOC_NUMBER2];
    inp_reg_mem[35] = ADCA_Results[ADC_SOC_NUMBER3];
    inp_reg_mem[36] = ADCA_Results[ADC_SOC_NUMBER4];
    inp_reg_mem[37] = ADCA_Results[ADC_SOC_NUMBER5];

    inp_reg_mem[38] = ADCB_Results[ADC_SOC_NUMBER0];
    inp_reg_mem[39] = ADCB_Results[ADC_SOC_NUMBER1];
    inp_reg_mem[40] = ADCB_Results[ADC_SOC_NUMBER2];
    inp_reg_mem[41] = ADCB_Results[ADC_SOC_NUMBER3];
    inp_reg_mem[42] = ADCB_Results[ADC_SOC_NUMBER4];
    inp_reg_mem[43] = ADCB_Results[ADC_SOC_NUMBER5];

    inp_reg_mem[44] = ADCC_Results[ADC_SOC_NUMBER0];
    inp_reg_mem[45] = ADCC_Results[ADC_SOC_NUMBER1];
    inp_reg_mem[46] = ADCC_Results[ADC_SOC_NUMBER2];
    inp_reg_mem[47] = ADCC_Results[ADC_SOC_NUMBER3];
    inp_reg_mem[48] = ADCC_Results[ADC_SOC_NUMBER4];
    inp_reg_mem[49] = ADCC_Results[ADC_SOC_NUMBER5];

    inp_reg_mem[50] = ADCD_Results[ADC_SOC_NUMBER0];
    inp_reg_mem[51] = ADCD_Results[ADC_SOC_NUMBER1];
    inp_reg_mem[52] = ADCD_Results[ADC_SOC_NUMBER2];
    inp_reg_mem[53] = ADCD_Results[ADC_SOC_NUMBER3];
    inp_reg_mem[54] = ADCD_Results[ADC_SOC_NUMBER4];
    inp_reg_mem[55] = ADCD_Results[ADC_SOC_NUMBER5];
}

typedef struct
{
    uint16_t u16Minimum;
    uint16_t u16Maximum;

}sAdcSineWaveMeasure_t;

sAdcSineWaveMeasure_t sAdcSineWaveMeasure[4][6];

void APPL_ADC_SineWaveEvaluate (uint16_t adcIndex, uint16_t adcSOC)
{
    uint16_t index;
    uint16_t value;
    sAdcSineWaveMeasure_t *psAdcSineWaveMeasure = &sAdcSineWaveMeasure[adcIndex][adcSOC];
    uint16_t *pData;

    switch (adcIndex)
    {
    case 0:
        pData = &ADCA_ResultBuffer[0];
        break;
    case 1:
        pData = &ADCB_ResultBuffer[0];
        break;
    case 2:
        pData = &ADCC_ResultBuffer[0];
        break;
    case 3:
        pData = &ADCD_ResultBuffer[0];
        break;
    }

    /* find min and max */
    psAdcSineWaveMeasure->u16Minimum = 0xFFFF;
    psAdcSineWaveMeasure->u16Maximum = 0;
    for(index = 0; index < APPL_ADC_RESULTS_BUFFER_SIZE; index++)
    {
        value = pData[index];
        if (psAdcSineWaveMeasure->u16Minimum > value)
        {
            psAdcSineWaveMeasure->u16Minimum = value;
        }
        if (psAdcSineWaveMeasure->u16Maximum < value)
        {
            psAdcSineWaveMeasure->u16Maximum = value;
        }
    }
}



#if 0
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
#endif





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
