#ifndef MBDATA_H
#define MBDATA_H

// Section: Included Files

#include <stddef.h>

#define HR_CNT                  256
#define IR_CNT                  256

#define HR_START_ADDR           (0x0300)
#define HR_END_ADDR             (HR_START_ADDR + HR_CNT - 1)
#define IR_START_ADDR           (0x0200)
#define IR_END_ADDR             (IR_START_ADDR + IR_CNT - 1)

//#define NO_MESSAGE_CNT  5
//
//#define INVALID_MESSAGE_CNT 0
//#define BUS_ERROR_CNT       1
//#define MODE_CHANGE_CNT     2
//#define SYSTEM_ERROR_CNT    3
//#define RX_BUFFER_OVERFLOW  4

extern uint16_t hold_reg_mem[HR_CNT];
extern uint16_t inp_reg_mem[IR_CNT];

void MBDATA_Init(void);

#endif  // MBDATA_H
