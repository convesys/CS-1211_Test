/*
 * appl_rtc.c
 *
 *  Created on: 21.09.2025
 *      Author: Deyan Todorov
 */

#include <stdint.h>
#include "appl_rtc.h"
#include "appl_helper.h"
#include "appl_i2ca.h"
#include "board.h"
#include "mbdata.h"

//
// Typedefs
//


//
// Variables
//
uint16_t RTC_ENABLE_MB_CONTROL = 1;
RTCControl RTCControl_inst;
uint16_t i2c_ack, rtc_buf[8];



void APPL_RTC_process(void)
{
    uint16_t i;
    if(RTC_ENABLE_MB_CONTROL){
        if(((0x0001 & I2C_CLK_REQUEST) == 0x0000) && ((0x0001 & I2C_CLK_CONFIRM) == 0x0001))RTCControl_inst.READ_CLOCK = 1;
        if(((0x0002 & I2C_CLK_REQUEST) == 0x0000) && ((0x0002 & I2C_CLK_CONFIRM) == 0x0002))RTCControl_inst.SET_CLOCK = 1;
        I2C_CLK_CONFIRM = I2C_CLK_REQUEST;
    }
    if (RTCControl_inst.READ_CLOCK)
    {
        i2c_ack = I2CA_Start();
        if(ACKBIT == i2c_ack)
        {
            i2c_ack = I2CA_Write(RTC_ADDR | I2C_WRITE);
            if(ACKBIT == i2c_ack)
            {
                i2c_ack = I2CA_Write(RTC_SEC_ADDR);
                if(ACKBIT == i2c_ack)
                {
                    i2c_ack = I2CA_Start();
                    if(ACKBIT == i2c_ack)
                    {
                        i2c_ack = I2CA_Write(RTC_ADDR | I2C_READ);
                        if(ACKBIT == i2c_ack)
                        {
                            for(i = 0; i < 7; i++)rtc_buf[i] = I2CA_Read(ACKBIT);
                            rtc_buf[i] = I2CA_Read(NACKBIT);
                        }
                    }
                }
            }
            I2CA_Stop();
            rtc_buf[0] &= RTC_SEC_GETM;
            rtc_buf[1] &= RTC_MIN_GETM;
            rtc_buf[2] &= RTC_HOUR_GETM;
            rtc_buf[3] &= RTC_WDAY_GETM;
            rtc_buf[4] &= RTC_DAY_GETM;
            rtc_buf[5] &= RTC_MNT_GETM;
            for(i = 0; i < 8; i++)hold_reg_mem[HR_CNT + 0x22 + i] = APPL_HELPER_bcd2bin(rtc_buf[i]);
        }
        RTCControl_inst.READ_CLOCK = 0;
    }
    if (RTCControl_inst.SET_CLOCK)
    {
        rtc_buf[0] = APPL_HELPER_bin2bcd((I2C_CLKS_SEC) & 0x00FF) | RTC_SEC_DEF;
        rtc_buf[1] = APPL_HELPER_bin2bcd((I2C_CLKS_MIN) & 0x00FF) | RTC_MIN_DEF;
        rtc_buf[2] = APPL_HELPER_bin2bcd((I2C_CLKS_HOUR) & 0x00FF) | RTC_HOUR_DEF;
        rtc_buf[3] = APPL_HELPER_bin2bcd((I2C_CLKS_WDAY) & 0x000F) | RTC_WDAY_DEF;
        rtc_buf[4] = APPL_HELPER_bin2bcd((I2C_CLKS_DAY) & 0x00FF) | RTC_DAY_DEF;
        rtc_buf[5] = APPL_HELPER_bin2bcd((I2C_CLKS_MNT) & 0x00FF) | RTC_MNT_DEF;
        rtc_buf[6] = APPL_HELPER_bin2bcd((I2C_CLKS_YEAR) & 0x00FF) | RTC_YEAR_DEF;
        rtc_buf[7] = RTC_CTR_DEF;
        i2c_ack = I2CA_Start();
        if(ACKBIT == i2c_ack)
        {
            i2c_ack = I2CA_Write(RTC_ADDR | I2C_WRITE);
            if(ACKBIT == i2c_ack)
            {
                i2c_ack = I2CA_Write(RTC_SEC_ADDR);
                if(ACKBIT == i2c_ack)
                {
                    for(i = 0; i < 8; i++){
                        i2c_ack = I2CA_Write(rtc_buf[i]);
                        if(NACKBIT == i2c_ack) break;
                    }
                }
            }
            I2CA_Stop();
        }
        RTCControl_inst.SET_CLOCK = 0;
    }
}
