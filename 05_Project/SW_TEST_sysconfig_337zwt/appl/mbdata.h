#ifndef MBDATA_H
#define MBDATA_H

// Section: Included Files

#include <stddef.h>

#define HR_CNT                  512
#define HR_CTR_CNT               64
//#define IR_CNT                  256

#define HR_START_ADDR           (0x0200)
#define HR_END_ADDR             (HR_START_ADDR + HR_CNT - 1 + HR_CTR_CNT)
//#define IR_START_ADDR           (0x0200)
//#define IR_END_ADDR             (IR_START_ADDR + IR_CNT - 1)

//#define NO_MESSAGE_CNT  5
//
//#define INVALID_MESSAGE_CNT 0
//#define BUS_ERROR_CNT       1
//#define MODE_CHANGE_CNT     2
//#define SYSTEM_ERROR_CNT    3
//#define RX_BUFFER_OVERFLOW  4

extern uint16_t hold_reg_mem[HR_CNT + HR_CTR_CNT];
#define inp_reg_mem hold_reg_mem
//extern uint16_t inp_reg_mem[IR_CNT];

#define ADC_REG_REQUEST         hold_reg_mem[HR_CNT + 0x00]  //0x0400
#define ADC_REG_CONFIRM         hold_reg_mem[HR_CNT + 0x01]  //0x0401
#define ADC_REG_FIXED_CHAN      hold_reg_mem[HR_CNT + 0x02]  //0x0402
#define CHECK_REG_REQUEST       hold_reg_mem[HR_CNT + 0x03]  //0x0403
#define CHECK_REG_CONFIRM       hold_reg_mem[HR_CNT + 0x04]  //0x0404
#define SPI_REG_REQUEST         hold_reg_mem[HR_CNT + 0x10]  //0x0410
#define SPI_REG_CONFIRM         hold_reg_mem[HR_CNT + 0x11]  //0x0411
#define SPI_CHECK_RESFL1        hold_reg_mem[HR_CNT + 0x12]  //0x0412
#define SPI_CHECK_RESFL2        hold_reg_mem[HR_CNT + 0x13]  //0x0413
#define SPI_CHECK_RESMR1        hold_reg_mem[HR_CNT + 0x14]  //0x0414
#define SPI_CHECK_RESMR2        hold_reg_mem[HR_CNT + 0x15]  //0x0415
#define I2C_CLK_REQUEST         hold_reg_mem[HR_CNT + 0x20]  //0x0420
#define I2C_CLK_CONFIRM         hold_reg_mem[HR_CNT + 0x21]  //0x0421
#define I2C_CLK_SEC             hold_reg_mem[HR_CNT + 0x22]  //0x0422
#define I2C_CLK_MIN             hold_reg_mem[HR_CNT + 0x23]  //0x0423
#define I2C_CLK_HOUR            hold_reg_mem[HR_CNT + 0x24]  //0x0424
#define I2C_CLK_WDAY            hold_reg_mem[HR_CNT + 0x25]  //0x0425
#define I2C_CLK_DAY             hold_reg_mem[HR_CNT + 0x26]  //0x0426
#define I2C_CLK_MNT             hold_reg_mem[HR_CNT + 0x27]  //0x0427
#define I2C_CLK_YEAR            hold_reg_mem[HR_CNT + 0x28]  //0x0428
#define I2C_CLKS_SEC            hold_reg_mem[HR_CNT + 0x29]  //0x0429
#define I2C_CLKS_MIN            hold_reg_mem[HR_CNT + 0x2A]  //0x042A
#define I2C_CLKS_HOUR           hold_reg_mem[HR_CNT + 0x2B]  //0x042B
#define I2C_CLKS_WDAY           hold_reg_mem[HR_CNT + 0x2C]  //0x042C
#define I2C_CLKS_DAY            hold_reg_mem[HR_CNT + 0x2D]  //0x042D
#define I2C_CLKS_MNT            hold_reg_mem[HR_CNT + 0x2E]  //0x042E
#define I2C_CLKS_YEAR           hold_reg_mem[HR_CNT + 0x2F]  //0x042F

void MBDATA_Init(void);

#endif  // MBDATA_H
