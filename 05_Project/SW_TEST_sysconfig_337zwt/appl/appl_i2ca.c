/*
 * appl_i2ca.c
 *
 *  Created on: 21.09.2025
 *      Author: Deyan Todorov
 */

#include <stdint.h>
#include "appl_i2ca.h"
#include "board.h"


#define Wait_Clk() {__asm(" RPT #120 || NOP");}

#define Wait_Sclk(){__asm(" RPT #60 || NOP");}

uint16_t i2ca_buf[I2C_DATA_SIZE];
uint16_t i2ca_timeout;

/******************************************************************************************/
uint16_t I2CA_Start(void){
    SCL(1);
    SDA(1);
    SET_TIMEOUT_I2C(i2ca_timeout); // Start timer for timeout
    while((!SCLI || !SDAI) && (i2ca_timeout > 0)){
        if(!SDAI){
            SCL(0);
            Wait_Sclk();
            SCL(1);
            Wait_Sclk();
        }
    }
    if(i2ca_timeout > 0){
        Wait_Sclk();
        SDA(0);                       // Pull SDA low
        Wait_Sclk();
        SCL(0);                       // Pull SCL low
        return ACKBIT;
    } else return NACKBIT;
}
/******************************************************************************************/
//void i2c1_repstart(void){
//}
/******************************************************************************************/
void I2CA_Stop(void){
    Wait_Sclk();
    SCL(0);                        // Ensure SCL is low
    SDA(0);                        // Ensure SDA low
    Wait_Sclk();
    SCL(1);                        // Pull SCL high
    SET_TIMEOUT_I2C(i2ca_timeout);// Start timer for timeout
    while(!SCLI && (i2ca_timeout > 0)){}; // wait for SCL becomes high or timeout expires
    Wait_Sclk();
    SDA(1);                   // Allow SDA to be pulled high
    SCL(1);                   // Allow SDA to be pulled high
    Wait_Clk();
}
/******************************************************************************************/
void I2C_Bit_Out(uint16_t data){
    SCL(0);                        // Ensure SCL is low
    if (data & 0x0080){             // Check if next bit is high
        SDA(1);                     // Release SDA to be pulled high
    } else {
        SDA(0);                    // Pull SDA low
    }
    Wait_Sclk();
    SCL(1);                        // Pull SCL high to clock bit
    Wait_Clk();
    SCL(0);                        // Pull SCL low for next bit
} 
/******************************************************************************************/
uint16_t I2C_Bit1_Out(uint16_t data){
    SCL(0);                        // Ensure SCL1 is low
    if (data & 0x80){                // Check if next bit is high
        SDA(1);               // Release SDA1 to be pulled high
    } else {
        SDA(0);                    // Pull SDA1 low
    }
    SCL(1);                        // Pull SCL1 high to clock bit
    SET_TIMEOUT_I2C(i2ca_timeout);// Start timer for timeout
    while(!SCLI && (i2ca_timeout > 0)){}; // wait for SCL becomes high or timeout expires
    Wait_Clk();
    SCL(0);                        // Pull SCL1 low for next bit
    if(i2ca_timeout > 0)return ACKBIT;
    else return NACKBIT;
} 
/******************************************************************************************/
void I2C_Bit_In(uint16_t *data){
    SCL(0);                        // Ensure SCL1 is low
    SDA(1);                   // Configure SDA1 as an input
    Wait_Sclk();
    SCL(1);                        // Bring SCL1 high to begin transfer
    *data &= 0xFE;                  // Assume next bit is low
    Wait_Clk();
    if (SDAI){                        // Check if SDA1 is high
        *data |= 0x01;              // If high, set next bit
    }
    SCL(0);                        // Bring SCL1 low again
} 
/******************************************************************************************/
void I2C_Bit1_In(uint16_t *data){
    SCL(0);                        // Ensure SCL1 is low
    SDA(1);                   // Configure SDA1 as an input
    Wait_Sclk();
    SCL(1);                        // Bring SCL1 high to begin transfer
    SET_TIMEOUT_I2C(i2ca_timeout);// Start timer for timeout
    while(!SCLI && (i2ca_timeout > 0)){}; // wait for SCL becomes high or timeout expires
    *data &= 0xFE;                  // Assume next bit is low
    Wait_Clk();
    if (SDAI){                        // Check if SDA1 is high
        *data |= 0x01;              // If high, set next bit
    }
    SCL(0);                        // Bring SCL1 low again
} 
/******************************************************************************************/
void I2C_Ack_In(uint16_t *data){
    SCL(0);                            // Ensure SCL1 is low
    SDA(1);
    Wait_Clk();
    SCL(1);                           // Bring SCL1 high to begin transfer
    SET_TIMEOUT_I2C(i2ca_timeout);// Start timer for timeout
    *data = ACKBIT;                      // Assume ACK or bit is low
    while(!SCLI && (i2ca_timeout > 0)){}; // wait for SCL becomes high or timeout expires
    
    if (SDAI || (0 == i2ca_timeout)){       // Check if NACK or SDA1 is high or timeout expired
        *data = NACKBIT;                // set NACKBIT
    }
    Wait_Clk();
    SCL(0);                           // Bring SCL1 low again
} 
/******************************************************************************************/
uint16_t I2CA_Write(uint16_t i2cWriteData){
    uint16_t i;                // Loop counter
    uint16_t ack;              // ACK bit
    ack = I2C_Bit1_Out(i2cWriteData);
    if(ack){
        return ack;
    } else {
        for (i = 0; i < 7; i++){         // Loop through each bit
            i2cWriteData = i2cWriteData << 1;       // Shift left for next bit
            I2C_Bit_Out(i2cWriteData);              // Output bit
        }
        I2C_Ack_In(&ack);                   // Input ACK bit
        return ack;
    }
}
/******************************************************************************************/
uint16_t I2CA_Read(uint16_t ack){
    uint16_t i;                // Loop counter
    uint16_t retval;           // Return value

    retval = 0;
    I2C_Bit1_In(&retval);            // Input bit
    for (i = 0; i < 7; i++){         // Loop through each bit
        retval = retval << 1;       // Shift left for next bit
        I2C_Bit_In(&retval);            // Input bit
    }
    I2C_Bit_Out(ack);                   // Output ACK/NAK bit

    return retval;
}
