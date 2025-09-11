#include <stdint.h>
#include "mbdata.h"

uint16_t hold_reg_mem[HR_CNT];
uint16_t inp_reg_mem[IR_CNT];

void MBDATA_Init(void){
    uint16_t i;
    for(i = 0; i < 0x200; i++){
        hold_reg_mem[i] = 0x1000 + i;
        inp_reg_mem[i] = 0x2000 + i;
    }
}


