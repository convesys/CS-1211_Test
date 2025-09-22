/*
 * appl_i2ca.h
 *
 *  Created on: 21.09.2025
 *      Author: Deyan Todorov
 */

#ifndef APPL_APPL_I2CA_H_
#define APPL_APPL_I2CA_H_

#include <stdint.h>

#define SCLI       (GPIO_readPin(I2CA_SCL))
#define SCL(val)   GPIO_writePin(I2CA_SCL, val)
#define SDAI       (GPIO_readPin(I2CA_SDA))
#define SDA(val)   GPIO_writePin(I2CA_SDA, val)
#define SET_TIMEOUT_I2C(tout) {while(tout != 20) tout = 20;}

#define I2C_DATA_SIZE 64

#define I2C_WRITE 0x0000
#define I2C_READ 0x0001
#define ACKBIT 0x0000
#define NACKBIT 0x0080

extern uint16_t i2ca_buf[I2C_DATA_SIZE];
extern uint16_t i2ca_timeout;

uint16_t I2CA_Start(void);
uint16_t I2CA_Write(uint16_t i2cWriteData);
uint16_t I2CA_Read(uint16_t nack);
void I2CA_Stop(void);

#endif /* APPL_APPL_I2CA_H_ */
