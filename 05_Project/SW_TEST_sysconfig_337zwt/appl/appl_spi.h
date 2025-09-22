/*
 * appl_spi.h
 *
 *  Created on: 20.09.2025
 *      Author: Deyan Todorov
 */

#ifndef APPL_APPL_SPI_H_
#define APPL_APPL_SPI_H_

#include <stdint.h>

#define SPIA_RDID 0x9F00
#define SPIC_RDID 0x9F00

//
// Types definition
//
typedef  union
{
    struct {
        uint16_t FLASH_Read_ID          :1;
        uint16_t MRMEM_Read_ID          :1;
        uint16_t reserved               :14;
    };
    uint16_t ui16;
} SPIControl;

extern SPIControl SPIControl_inst;

void APPL_SPI_process(void);

#endif /* APPL_APPL_SPI_H_ */
