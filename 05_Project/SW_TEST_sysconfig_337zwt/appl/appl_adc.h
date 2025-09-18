/*
 * appl_adc.h
 *
 *  Created on: 13.09.2025
 *      Author: Dimitar Lilov
 */

#ifndef APPL_APPL_ADC_H_
#define APPL_APPL_ADC_H_

//
// Include Files
//

#include <stdint.h>


//
// Configuration Definitions
//

#define APPL_ADC_RESULTS_BUFFER_SIZE                    4096    //buffer size for storing conversion results for 1 channel
#define APPL_ADC_RESULTS_OFFS                           2048

#define APPL_ADC_RESOLUTION                             12
// 12 for 12-bit conversion resolution, which supports (ADC_MODE_SINGLE_ENDED)
// Sample on single pin (VREFLO is the low reference)
// Or 16 for 16-bit conversion resolution, which supports (ADC_MODE_DIFFERENTIAL)
// Sample on pair of pins (difference between pins is converted, subject to
// common mode voltage requirements; see the device data manual)


#define APPL_ADC_FIRST_VALID_SAMPLE_BUFFER_INDEX        4       //first valid sample index in buffer - mostly because small sampling time and if adc continuous conversions not started before dma

#define APPL_ADC_CHANNEL_SAMPLING_SYSCLK_TICKS          20      //20->100ns


#define APPL_ADC_START_DMA_DELAYED_ADC_CYCLES           1

#define APPL_ADC_FILTER_HALF_SINEWAVE_CHANGE            16

//
// Variable External Usage
//
extern uint16_t APPL_ADC_timer_start_adc[4];
extern uint16_t APPL_ADC_timer_count_adc[4];

//
// Functions External Usage
//
void APPL_ADC_init(void);
void APPL_ADC_process(void);
void APPL_ADC_StopTriggering(uint32_t adcBase);
void APPL_ADC_Fill(void);


#endif /* APPL_APPL_ADC_H_ */
