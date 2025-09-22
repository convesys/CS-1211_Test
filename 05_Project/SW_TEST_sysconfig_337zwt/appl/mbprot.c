#include "mbprot.h"
#include "mbprot_types.h"
#include "mbcrc.h"
#include "mbdata.h"
#include "board.h"

#define MB_CONFIG_TX_BYTEQ_LENGTH (256)
#define MB_CONFIG_RX_BYTEQ_LENGTH (256)

static uint16_t ui16_Tx1Buff[MB_CONFIG_TX_BYTEQ_LENGTH];
static uint16_t ui16_Rx1Buff[MB_CONFIG_RX_BYTEQ_LENGTH];
static uint16_t ui16_Mb1Buff[MB_CONFIG_RX_BYTEQ_LENGTH];

static uint16_t ui16_Rx1BuffIdx, ui16_Rx1Cnt, ui16_Tx1BuffIdx, ui16_Tx1Char2Send;
static MBStatus mb1_inst;

static uint16_t ui16_MBT15_Tmr;
static uint16_t ui16_MBT35_Tmr;

uint16_t MB_CheckAddr(uint16_t *addr, uint16_t saddr, uint16_t eaddr, uint16_t cnt){
    *addr = ui16_Mb1Buff[2]*256 + ui16_Mb1Buff[3];
    if((*addr >= saddr) && (*addr + cnt <= eaddr)) {
        *addr = *addr - saddr;
        return DATA_OK;
    } else return BAD_ADDR;
}

uint16_t MB_CheckCnt(uint16_t *cnt){
    uint16_t bcnt;
    *cnt = ui16_Mb1Buff[4]*256 + ui16_Mb1Buff[5];
    
    if((*cnt < 1) || (*cnt > 0x007D)) return BAD_CNT;
    if(0x10 == ui16_Mb1Buff[1]){
        bcnt = *cnt;
        bcnt *= 2;
        if(bcnt != ui16_Mb1Buff[6]) return BAD_CNT;
    };
    return DATA_OK;
}

uint16_t MB_CheckAddr_Cnt(uint16_t *addr, uint16_t saddr, uint16_t eaddr, uint16_t *cnt){
    uint16_t res;
    res = MB_CheckCnt(cnt);
    if(!res) res = MB_CheckAddr(addr, saddr, eaddr, *cnt);
    if(res){ 
        ui16_Tx1Buff[0] = ui16_Mb1Buff[0];
        ui16_Tx1Buff[1] = ui16_Mb1Buff[1] | 0x80;
        ui16_Tx1Buff[2] = res;
        make_crc16(ui16_Tx1Buff, 3);
        ui16_Tx1Char2Send = 5;
    }
    return res;
}

void MB_Read_Data(uint16_t *mem, uint16_t addr, uint16_t cnt){
    uint16_t i, bcnt;
    uint16_t j;
    ui16_Tx1Buff[0] = ui16_Mb1Buff[0];
    ui16_Tx1Buff[1] = ui16_Mb1Buff[1];
    ui16_Tx1Buff[2] = bcnt = cnt * 2;
    for (i = 0; i < cnt; i++){
        j = addr + i;
        ui16_Tx1Buff[i*2+3] = mem[j] / 256;
        ui16_Tx1Buff[i*2+4] = mem[j];
    }    
    make_crc16(ui16_Tx1Buff, bcnt + 3);
    ui16_Tx1Char2Send = bcnt + 5;
}

void MB_Write_Data(uint16_t *mem, uint16_t addr, uint16_t cnt){
    uint16_t i;
    ui16_Tx1Buff[0] = ui16_Mb1Buff[0];
    ui16_Tx1Buff[1] = ui16_Mb1Buff[1];
    ui16_Tx1Buff[2] = ui16_Mb1Buff[2];
    ui16_Tx1Buff[3] = ui16_Mb1Buff[3];
    ui16_Tx1Buff[4] = ui16_Mb1Buff[4];
    ui16_Tx1Buff[5] = ui16_Mb1Buff[5];
    if(0xFF == cnt){
        //GPIO_togglePin(LED1_GPIO);
        mem[addr] = ui16_Mb1Buff[4] * 256 + ui16_Mb1Buff[5];
    } else {
        for (i = 0; i < cnt; i++) mem[addr + i] = ui16_Mb1Buff[i*2+7] * 256 +  ui16_Mb1Buff[i*2+8];
    }
    make_crc16(ui16_Tx1Buff, 6);
    ui16_Tx1Char2Send = 8;
}

void MB_CheckSend(void){
    if(!mb1_inst.sendResponse){
        mb1_inst.responseReady = true;
        if(mb1_inst.sendResponse){
            mb1_inst.responseReady = false;
            Interrupt_enable(INT_SCIA_inst_TX);
        }
    } else {
        mb1_inst.responseReady = false;
        Interrupt_enable(INT_SCIA_inst_TX);
    }
    mb1_inst.check = false;
}

void MB_Main(void){
    uint16_t addr;
    uint16_t cnt;
    if(mb1_inst.check){
        
        if(DATA_OK == check_crc16(ui16_Mb1Buff, ui16_Rx1Cnt)){
            if(MB_ADDRESS == ui16_Mb1Buff[0]){

                switch(ui16_Mb1Buff[1]){
                    case 0x03:
                        if(!MB_CheckAddr_Cnt(&addr, HR_START_ADDR, HR_END_ADDR, &cnt))MB_Read_Data(hold_reg_mem, addr, cnt);
//                        if(!MB_CheckAddr_Cnt(&addr, IR_START_ADDR, IR_END_ADDR, &cnt))MB_Read_Data(inp_reg_mem, addr, cnt);
                        break;
                    case 0x04:
                        if(!MB_CheckAddr_Cnt(&addr, HR_START_ADDR, HR_END_ADDR, &cnt))MB_Read_Data(hold_reg_mem, addr, cnt);
//                        if(!MB_CheckAddr_Cnt(&addr, IR_START_ADDR, IR_END_ADDR, &cnt))MB_Read_Data(inp_reg_mem, addr, cnt);
                        break;
                    case 0x06:
                        if(!MB_CheckAddr(&addr, HR_START_ADDR, HR_END_ADDR, 1)){
                            MB_Write_Data(hold_reg_mem, addr, 0xFF);
                        }
                        break;
                    case 0x10:
                        if(!MB_CheckAddr_Cnt(&addr, HR_START_ADDR, HR_END_ADDR, &cnt))MB_Write_Data(hold_reg_mem, addr, cnt);
                        break;
                    default :
                        ui16_Tx1Buff[0] = ui16_Mb1Buff[0];
                        ui16_Tx1Buff[1] = ui16_Mb1Buff[1] | 0x80;
                        ui16_Tx1Buff[2] = 0x01;
                        make_crc16(ui16_Tx1Buff, 3);
                        ui16_Tx1Char2Send = 5;
                        
                }
                MB_CheckSend();
            } 
        } 

    }
}

void MB_Check_Tmr(void){
    uint16_t i;
    
    if(ui16_MBT15_Tmr < MB_T15){
        if(MB_T15 <= ++ui16_MBT15_Tmr){
            mb1_inst.inMessage = false;
            if((ui16_Rx1BuffIdx > 3) && (MB_ADDRESS == ui16_Rx1Buff[0])){
                ui16_Rx1Cnt = ui16_Rx1BuffIdx;
                ui16_MBT35_Tmr = 0;
                for(i=0;i<ui16_Rx1Cnt;i++)ui16_Mb1Buff[i] = ui16_Rx1Buff[i];
                mb1_inst.check = true;
                ui16_Tx1BuffIdx = 0;
            }
        }
    }
    if(ui16_MBT35_Tmr < MB_T35){
        if(MB_T35 <= ++ui16_MBT35_Tmr){
            if(mb1_inst.responseReady){
                Interrupt_enable(INT_SCIA_inst_TX);
                mb1_inst.responseReady = false;
            } else if(mb1_inst.check)mb1_inst.sendResponse = true;
        }
    }

}

void MBTXInterrupt(void)
{
    while((SCI_getTxFIFOStatus(SCIA_BASE) != SCI_FIFO_TX16) && (ui16_Tx1BuffIdx < ui16_Tx1Char2Send))
        SCI_writeCharNonBlocking(SCIA_BASE, ui16_Tx1Buff[ui16_Tx1BuffIdx++]);
    if(ui16_Tx1BuffIdx >= ui16_Tx1Char2Send) {
        Interrupt_disable(INT_SCIA_inst_TX);
        GPIO_writePin(LED4_GPIO, 1);
    }
}

void MBRXInterrupt(void)
{
    uint16_t data, stat;
    GPIO_writePin(LED4_GPIO, 0);
    while(SCI_getRxFIFOStatus(SCIA_BASE)){
        stat = SCI_getRxStatus(SCIA_BASE);
        data = SCI_readCharNonBlocking(SCIA_BASE);
        if(0x0000 == (stat & SCI_RXSTATUS_ERROR)){
            if(!mb1_inst.inMessage){
                ui16_Rx1BuffIdx = 0;
                mb1_inst.inMessage = true;
            }
            ui16_Rx1Buff[ui16_Rx1BuffIdx++] = data;
            ui16_MBT15_Tmr = 0;
        } else {
            if(mb1_inst.inMessage){
                mb1_inst.inMessage = false;
            }
        }
    }
    if(SCI_getOverflowStatus(SCIA_BASE)){
        SCI_clearOverflowStatus(SCIA_BASE);
        if(mb1_inst.inMessage){
            mb1_inst.inMessage = false;
        }
    }
}
