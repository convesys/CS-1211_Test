/*
 * appl_rtc.h
 *
 *  Created on: 21.09.2025
 *      Author: Deyan Todorov
 */

#ifndef APPL_APPL_RTC_H_
#define APPL_APPL_RTC_H_

#include <stdint.h>

#define RTC_ADDR        0x00DE
#define RTC_READ        0x0001

#define RTC_SEC_ADDR    0x0000


#define RTC_SEC_DEF     0x0080
#define RTC_MIN_DEF     0x0000
#define RTC_HOUR_DEF    0x0000
#define RTC_WDAY_DEF    0x0008
#define RTC_DAY_DEF     0x0000
#define RTC_MNT_DEF     0x0000
#define RTC_YEAR_DEF    0x0000
#define RTC_CTR_DEF     0x0080


#define RTC_SEC_GETM    0x007F
#define RTC_MIN_GETM    0x007F
#define RTC_HOUR_GETM   0x003F
#define RTC_WDAY_GETM   0x0007
#define RTC_DAY_GETM    0x003F
#define RTC_MNT_GETM    0x001F


//
// Types definition
//
typedef  union
{
    struct {
        uint16_t READ_CLOCK             :1;
        uint16_t SET_CLOCK              :1;
        uint16_t reserved               :14;
    };
    uint16_t ui16;
} RTCControl;

extern RTCControl RTCControl_inst;

void APPL_RTC_process(void);

#endif /* APPL_APPL_RTC_H_ */
