#ifndef MBPROT_TYPES_H
#define MBPROT_TYPES_H

#include <stdint.h>

#define MB_T15   15  //750 / 50 = 15
#define MB_T35   35  //1750 / 50 = 35


typedef  union
{
    struct {
        uint16_t inMessage :1;
        uint16_t check : 1;
        uint16_t responseReady :1;
        uint16_t sendResponse : 1;
        uint16_t reserved :12;
    };
    uint16_t ui16;
} MBStatus;

#endif // MBPROT_TYPES_H


