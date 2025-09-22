/*
 * appl_spi.c
 *
 *  Created on: 20.09.2025
 *      Author: Deyan Todorov
 */

#include <stdint.h>
#include "appl_spi.h"
#include "board.h"

#include "mbdata.h"

//
// Typedefs
//


//
// Variables
//
uint16_t SPI_ENABLE_MB_CONTROL = 1;
SPIControl SPIControl_inst;
uint16_t FLASH_data[2], MRMEM_data[2];

void APPL_SPI_process(void)
{
    if(SPI_ENABLE_MB_CONTROL){
        if(((0x0001 & SPI_REG_REQUEST) == 0x0000) && ((0x0001 & SPI_REG_CONFIRM) == 0x0001))SPIControl_inst.FLASH_Read_ID = 1;
        if(((0x0002 & SPI_REG_REQUEST) == 0x0000) && ((0x0002 & SPI_REG_CONFIRM) == 0x0002))SPIControl_inst.MRMEM_Read_ID = 1;
        SPI_REG_CONFIRM = SPI_REG_REQUEST;
    }
    if (SPIControl_inst.FLASH_Read_ID)
    {
        GPIO_writePin(inst_GPIO61, 0);
        FLASH_data[0] = FLASH_data[1] = 0;
        SPI_writeDataBlockingNonFIFO(SPIA_inst_BASE, SPIA_RDID);
        FLASH_data[0] = SPI_readDataBlockingNonFIFO(SPIA_inst_BASE);
        SPI_writeDataBlockingNonFIFO(SPIA_inst_BASE, 0);
        FLASH_data[0] = SPI_readDataBlockingNonFIFO(SPIA_inst_BASE);
        SPI_writeDataBlockingNonFIFO(SPIA_inst_BASE, 0);
        FLASH_data[0] = SPI_readDataBlockingNonFIFO(SPIA_inst_BASE);
        SPI_writeDataBlockingNonFIFO(SPIA_inst_BASE, 0);
        FLASH_data[1] = SPI_readDataBlockingNonFIFO(SPIA_inst_BASE);
        GPIO_writePin(inst_GPIO61, 1);
        SPIControl_inst.FLASH_Read_ID = 0;
        SPI_CHECK_RESFL1 = FLASH_data[0];
        SPI_CHECK_RESFL2 = FLASH_data[1];
    }
    if (SPIControl_inst.MRMEM_Read_ID)
    {
        GPIO_writePin(inst_GPIO72, 0);
        MRMEM_data[0] = MRMEM_data[1] = 0;
        SPI_writeDataBlockingNonFIFO(SPIC_inst_BASE, SPIC_RDID);
        MRMEM_data[0] = SPI_readDataBlockingNonFIFO(SPIC_inst_BASE);
        SPI_writeDataBlockingNonFIFO(SPIC_inst_BASE, 0);
        MRMEM_data[0] = (SPI_readDataBlockingNonFIFO(SPIC_inst_BASE) << 8) & 0xFF00;
        SPI_writeDataBlockingNonFIFO(SPIC_inst_BASE, 0);
        MRMEM_data[0] |= SPI_readDataBlockingNonFIFO(SPIC_inst_BASE);
        SPI_writeDataBlockingNonFIFO(SPIC_inst_BASE, 0);
        MRMEM_data[1] = (SPI_readDataBlockingNonFIFO(SPIC_inst_BASE) << 8) & 0xFF00;
        SPI_writeDataBlockingNonFIFO(SPIC_inst_BASE, 0);
        MRMEM_data[1] |= SPI_readDataBlockingNonFIFO(SPIC_inst_BASE);
        GPIO_writePin(inst_GPIO72, 1);
        SPIControl_inst.MRMEM_Read_ID = 0;
        SPI_CHECK_RESMR1 = MRMEM_data[0];
        SPI_CHECK_RESMR2 = MRMEM_data[1];
    }
}
