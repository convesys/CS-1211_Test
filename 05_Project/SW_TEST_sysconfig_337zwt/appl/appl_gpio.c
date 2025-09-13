/*
 * appl_gpio.c
 *
 *  Created on: 13.09.2025
 *      Author: Dimitar Lilov
 */

#include <stdint.h>
#include "mbdata.h"
#include "board.h"

//
// Read All GPIO Pins
//
void APPL_GPIO_Fill(void)
{

    inp_reg_mem[0]  = GPIO_readPin(inst_GPIO148) << 0;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO147) << 1;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO145) << 2;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO146) << 3;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO149) << 4;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO150) << 5;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO153) << 6;
    inp_reg_mem[0] |= GPIO_readPin(inst_GPIO154) << 7;

    inp_reg_mem[1]  = GPIO_readPin(inst_GPIO157) << 0;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO158) << 1;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO161) << 2;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO162) << 3;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO36) << 4;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO130) << 5;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO20) << 6;
    inp_reg_mem[1] |= GPIO_readPin(inst_GPIO10) << 7;

    inp_reg_mem[2]  = GPIO_readPin(inst_GPIO8) << 0;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO131) << 1;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO132) << 2;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO133) << 3;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO134) << 4;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO135) << 5;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO136) << 6;
    inp_reg_mem[2] |= GPIO_readPin(inst_GPIO32) << 7;

    inp_reg_mem[3]  = GPIO_readPin(inst_GPIO33) << 0;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO119) << 1;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO125) << 2;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO126) << 3;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO127) << 4;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO128) << 5;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO129) << 6;
    inp_reg_mem[3] |= GPIO_readPin(inst_GPIO160) << 7;

    inp_reg_mem[4]  = GPIO_readPin(inst_GPIO159) << 0;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO21) << 1;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO99) << 2;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO98) << 3;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO104) << 4;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO105) << 5;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO9) << 6;
    inp_reg_mem[4] |= GPIO_readPin(inst_GPIO30) << 7;

    inp_reg_mem[5]  = GPIO_readPin(inst_GPIO11) << 0;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO110) << 1;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO111) << 2;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO102) << 3;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO100) << 4;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO101) << 5;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO108) << 6;
    inp_reg_mem[5] |= GPIO_readPin(inst_GPIO55) << 7;

    inp_reg_mem[6]  = GPIO_readPin(inst_GPIO57) << 0;
//    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO0) << 1;
//    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO1) << 2;
    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO84) << 3;   //Note!: Boot pin
    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO43) << 4;
    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO27) << 5;
//    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO_NA) << 6;
//    inp_reg_mem[6] |= GPIO_readPin(inst_GPIO_NA) << 7;

    inp_reg_mem[7]  = GPIO_readPin(inst_GPIO156) << 0;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO155) << 1;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO152) << 2;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO151) << 3;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO26) << 4;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO142) << 5;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO141) << 6;
    inp_reg_mem[7] |= GPIO_readPin(inst_GPIO138) << 7;

    inp_reg_mem[8]  = GPIO_readPin(inst_GPIO137) << 0;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO166) << 1;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO165) << 2;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO163) << 3;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO164) << 4;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO167) << 5;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO168) << 6;
    inp_reg_mem[8] |= GPIO_readPin(inst_GPIO139) << 7;

    inp_reg_mem[9]  = GPIO_readPin(inst_GPIO140) << 0;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO143) << 1;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO144) << 2;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO62) << 3;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO56) << 4;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO54) << 5;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO103) << 6;
    inp_reg_mem[9] |= GPIO_readPin(inst_GPIO53) << 7;


    inp_reg_mem[10]  = GPIO_readPin(inst_GPIO66) << 0;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO2) << 1;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO3) << 2;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO22) << 3;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO23) << 4;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO96) << 5;
    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO95) << 6;
//    inp_reg_mem[10] |= GPIO_readPin(inst_GPIO29) << 7;

    inp_reg_mem[11]  = 0;   //GPIO_readPin(inst_GPIO28) << 0;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO93) << 1;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO94) << 2;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO97) << 3;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO15) << 4;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO12) << 5;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO13) << 6;
    inp_reg_mem[11] |= GPIO_readPin(inst_GPIO14) << 7;


    inp_reg_mem[12]  = GPIO_readPin(inst_GPIO72) << 0;  //Note! Boot Pin
    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO25) << 1;
    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO24) << 2;
//    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO_NA) << 3;
//    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO_NA) << 4;
//    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO_NA) << 5;
//    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO_NA) << 6;
//    inp_reg_mem[12] |= GPIO_readPin(inst_GPIO_NA) << 7;

//    inp_reg_mem[13]  = GPIO_readPin(inst_GPIO_NA) << 0;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 1;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 2;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 3;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 4;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 5;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 6;
//    inp_reg_mem[13] |= GPIO_readPin(inst_GPIO_NA) << 7;

    inp_reg_mem[14]  = 0;   //GPIO_readPin(inst_GPIO_NA) << 0;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO5) << 1;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO4) << 2;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO7) << 3;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO6) << 4;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO65) << 5;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO63) << 6;
    inp_reg_mem[14] |= GPIO_readPin(inst_GPIO64) << 7;


}

