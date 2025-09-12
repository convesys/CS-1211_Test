/*
 * appl.c
 *
 *  Created on: 1 Sep 2025
 *      Author: Deyan Todorov
 */

//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"
#include "mbprot.h"
#include "mbdata.h"

//
// Configuration Definitions
//

#define RESULTS_BUFFER_SIZE     1024 //buffer size for storing conversion results 1 channel

#define APPL_ADC_RESOLUTION       12
// 12 for 12-bit conversion resolution, which supports (ADC_MODE_SINGLE_ENDED)
// Sample on single pin (VREFLO is the low reference)
// Or 16 for 16-bit conversion resolution, which supports (ADC_MODE_DIFFERENTIAL)
// Sample on pair of pins (difference between pins is converted, subject to
// common mode voltage requirements; see the device data manual)



//
// Variables
//

uint16_t bManualMeasureConfigure = 1;
uint16_t bManualMeasureRequest = 0;
uint16_t bManualMeasureStop = 0;

uint16_t ui16_tmr0_cntr;
uint16_t ui16_snd_cntr, ui16_snd_cntrp;
uint16_t ui16_dma0_cntr;

#pragma DATA_SECTION(ADCA_ResultBuffer, "ramgs0_15");
#pragma DATA_SECTION(ADCB_ResultBuffer, "ramgs0_15");
#pragma DATA_SECTION(ADCC_ResultBuffer, "ramgs0_15");
#pragma DATA_SECTION(ADCD_ResultBuffer, "ramgs0_15");
uint16_t ADCA_ResultBuffer[RESULTS_BUFFER_SIZE + 16];   //value index 0 could be stale read by dma when ADC prescale Ratio > 2 - take data from index 1 - read 16 more values to compensate
uint16_t ADCB_ResultBuffer[RESULTS_BUFFER_SIZE + 16];   //value index 0 could be stale read by dma when ADC prescale Ratio > 2
uint16_t ADCC_ResultBuffer[RESULTS_BUFFER_SIZE + 16];   //value index 0 could be stale read by dma when ADC prescale Ratio > 2
uint16_t ADCD_ResultBuffer[RESULTS_BUFFER_SIZE + 16];   //value index 0 could be stale read by dma when ADC prescale Ratio > 2

const void * ADCA_ResultBufferPtr = ADCA_ResultBuffer;
const void * ADCB_ResultBufferPtr = ADCB_ResultBuffer;
const void * ADCC_ResultBufferPtr = ADCC_ResultBuffer;
const void * ADCD_ResultBufferPtr = ADCD_ResultBuffer;

const void * ADCA_ResultRegisterPtr = (const void *)ADCARESULT_BASE;
const void * ADCB_ResultRegisterPtr = (const void *)ADCBRESULT_BASE;
const void * ADCC_ResultRegisterPtr = (const void *)ADCCRESULT_BASE;
const void * ADCD_ResultRegisterPtr = (const void *)ADCDRESULT_BASE;

uint16_t ADCA_InterruptCount[4];
uint16_t ADCB_InterruptCount[4];
uint16_t ADCC_InterruptCount[4];
uint16_t ADCD_InterruptCount[4];

/* timer resolution set to 0.5 us */
uint16_t timer_start_adc = 0;
uint16_t timer_final_adc = 0;
uint16_t timer_final_dma = 0;
uint16_t timer_count_adc = 0;
uint16_t timer_count_dma = 0;
uint16_t timer_count_adc_dma = 0;

//
// Function Prototypes
//
void APPL_ADC_Start(void);
void APPL_ADC_Fill(void);

void APPL_ADC_MeasureStart(void);
void APPL_ADC_MeasureStop(void);
void APPL_DMA_ADCA(void);


//
// Interrupt Functions
//

//
// dmach1ISR - This is called at the end of the DMA transfer, the conversions
//              are stopped by removing the trigger of the first SOC from
//              the last.
//
__interrupt void INT_DMA0_inst_ISR(void)
{
    ui16_dma0_cntr++;
    //
    // Stop the ADC by removing the trigger for SOC0
    //
    ADC_setInterruptSOCTrigger(ADCA_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);
    timer_final_dma = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    timer_count_dma = timer_start_adc - timer_final_dma;
    timer_count_adc_dma = timer_count_adc;
    //APPL_ADC_MeasureStop();

    Interrupt_clearACKGroup(INT_DMA0_inst_INTERRUPT_ACK_GROUP);
}

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
    timer_final_adc = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    timer_count_adc = timer_start_adc - timer_final_adc;

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


__interrupt void INT_CPUTIMER0_inst_ISR(void)
{
    MB_Check_Tmr();
    if(20000 <= ++ui16_tmr0_cntr){
        ui16_tmr0_cntr = 0;

        //APPL_ADC_Fill();
        //APPL_ADC_Start();

        //APPL_ADC_MeasureStart();

        ui16_snd_cntr++;
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
    Interrupt_clearACKGroup(INT_CPUTIMER0_inst_INTERRUPT_ACK_GROUP);
}

uint16_t ui16_readval, ui16_readcnt;

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

//
// Application Functions
//

uint16_t hex_str(uint16_t val)
{
    val &= 0x0000F;
    if(val > 9){
        val -= 10;
        val += 'A';
    } else val += '0';
    return val;
}

void APPL_GPIO_Fill(void)
{

    inp_reg_mem[0]  = GPIO_readPin(inst_GPIO148) << 0;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO147) << 1;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO145) << 2;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO146) << 3;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO149) << 4;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO150) << 5;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO153) << 6;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO154) << 7;

    inp_reg_mem[1]  = GPIO_readPin(inst_GPIO157) << 0;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO158) << 1;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO161) << 2;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO162) << 3;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO36) << 4;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO130) << 5;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO20) << 6;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO10) << 7;

    inp_reg_mem[2]  = GPIO_readPin(inst_GPIO8) << 0;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO131) << 1;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO132) << 2;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO133) << 3;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO134) << 4;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO135) << 5;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO136) << 6;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO32) << 7;

    inp_reg_mem[3]  = GPIO_readPin(inst_GPIO33) << 0;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO119) << 1;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO125) << 2;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO126) << 3;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO127) << 4;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO128) << 5;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO129) << 6;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO160) << 7;

    inp_reg_mem[4]  = GPIO_readPin(inst_GPIO159) << 0;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO21) << 1;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO99) << 2;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO98) << 3;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO104) << 4;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO105) << 5;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO9) << 6;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO30) << 7;

    inp_reg_mem[5]  = GPIO_readPin(inst_GPIO11) << 0;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO110) << 1;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO111) << 2;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO102) << 3;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO100) << 4;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO101) << 5;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO108) << 6;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO55) << 7;

    inp_reg_mem[6]  = GPIO_readPin(inst_GPIO57) << 0;
//    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO0) << 1;
//    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO1) << 2;
    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO84) << 3;   //Note!: Boot pin
    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO43) << 4;
    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO27) << 5;
//    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO_NA) << 6;
//    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO_NA) << 7;

    inp_reg_mem[7]  = GPIO_readPin(inst_GPIO156) << 0;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO155) << 1;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO152) << 2;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO151) << 3;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO26) << 4;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO142) << 5;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO141) << 6;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO138) << 7;

    inp_reg_mem[8]  = GPIO_readPin(inst_GPIO137) << 0;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO166) << 1;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO165) << 2;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO163) << 3;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO164) << 4;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO167) << 5;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO168) << 6;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO139) << 7;

    inp_reg_mem[9]  = GPIO_readPin(inst_GPIO140) << 0;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO143) << 1;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO144) << 2;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO62) << 3;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO56) << 4;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO54) << 5;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO103) << 6;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO53) << 7;


    inp_reg_mem[10]  = GPIO_readPin(inst_GPIO66) << 0;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO2) << 1;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO3) << 2;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO22) << 3;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO23) << 4;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO96) << 5;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO95) << 6;
//    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO29) << 7;

    inp_reg_mem[11]  = 0;   //GPIO_readPin(inst_GPIO28) << 0;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO93) << 1;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO94) << 2;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO97) << 3;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO15) << 4;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO12) << 5;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO13) << 6;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO14) << 7;


    inp_reg_mem[12]  = GPIO_readPin(inst_GPIO72) << 0;  //Note! Boot Pin
    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO25) << 1;
    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO24) << 2;
//    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO_NA) << 3;
//    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO_NA) << 4;
//    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO_NA) << 5;
//    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO_NA) << 6;
//    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO_NA) << 7;

//    inp_reg_mem[13]  = GPIO_readPin(inst_GPIO_NA) << 0;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 1;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 2;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 3;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 4;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 5;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 6;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 7;

    inp_reg_mem[14]  = 0;   //GPIO_readPin(inst_GPIO_NA) << 0;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO5) << 1;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO4) << 2;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO7) << 3;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO6) << 4;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO65) << 5;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO63) << 6;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO64) << 7;


}

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
    DMA_configTransfer(DMA_CH1_BASE, (RESULTS_BUFFER_SIZE >> 4), -14, 2);
    DMA_configMode(DMA_CH1_BASE, DMA_TRIGGER_ADCA2,
                   (DMA_CFG_ONESHOT_DISABLE | DMA_CFG_CONTINUOUS_DISABLE |
                    DMA_CFG_SIZE_32BIT));

    DMA_enableTrigger(DMA_CH1_BASE);
    DMA_disableOverrunInterrupt(DMA_CH1_BASE);
    DMA_setInterruptMode(DMA_CH1_BASE, DMA_INT_AT_END);
    DMA_enableInterrupt(DMA_CH1_BASE);

}


void APPL_main(void)
{
//    if(ui16_snd_cntr != ui16_snd_cntrp){
//        SCI_writeCharNonBlocking(SCIA_BASE, '0');
//        SCI_writeCharNonBlocking(SCIA_BASE, 'x');
//        SCI_writeCharNonBlocking(SCIA_BASE, hex_str(ui16_snd_cntr >> 12));
//        SCI_writeCharNonBlocking(SCIA_BASE, hex_str(ui16_snd_cntr >> 8));
//        SCI_writeCharNonBlocking(SCIA_BASE, hex_str(ui16_snd_cntr >> 4));
//        SCI_writeCharNonBlocking(SCIA_BASE, hex_str(ui16_snd_cntr));
//        SCI_writeCharNonBlocking(SCIA_BASE, '\r');
//        SCI_writeCharNonBlocking(SCIA_BASE, '\n');
//        ui16_snd_cntrp = ui16_snd_cntr;
//    }
    uint16_t timer_start = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    //APPL_ADC_Start();
    APPL_GPIO_Fill();
    //APPL_ADC_Fill();
    uint16_t timer_final = CPUTimer_getTimerCount(CPUTIMER1_BASE);
    inp_reg_mem[100] =  timer_start - timer_final;
//    if(timer_final >= timer_start) inp_reg_mem[100] = timer_final - timer_start;
//    else inp_reg_mem[100] = timer_final +(65535 - timer_start) + 1;
    MB_Main();

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
        timer_start_adc = CPUTimer_getTimerCount(CPUTIMER1_BASE);
        bManualMeasureRequest = 0;
    }
    if (bManualMeasureStop)
    {
        ADC_setInterruptSOCTrigger(ADCA_BASE, ADC_SOC_NUMBER0, ADC_INT_SOC_TRIGGER_NONE);
        bManualMeasureStop = 0;
    }


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
    EINT;
    ERTM;
}

//
// End of File
//


