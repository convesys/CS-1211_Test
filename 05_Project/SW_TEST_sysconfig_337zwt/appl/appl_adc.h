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

#define APPL_ADC_RESULTS_BUFFER_SIZE    1024    //buffer size for storing conversion results for 1 channel

#define APPL_ADC_RESOLUTION             12
// 12 for 12-bit conversion resolution, which supports (ADC_MODE_SINGLE_ENDED)
// Sample on single pin (VREFLO is the low reference)
// Or 16 for 16-bit conversion resolution, which supports (ADC_MODE_DIFFERENTIAL)
// Sample on pair of pins (difference between pins is converted, subject to
// common mode voltage requirements; see the device data manual)

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


#endif /* APPL_APPL_ADC_H_ */
