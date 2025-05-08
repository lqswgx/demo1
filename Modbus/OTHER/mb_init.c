/*
 * @Author: LQS
 * @Date: 2025-05-06 22:06:38
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 23:39:06
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\OTHER\mb_init.c
 * @Description: 
 */
#include "mb_init.h"
#include "flash.h"

MB_PROTOCOL mb_protocol;

uint16_t InputRegs[INPUT_REG_COUNT];
uint16_t HoldingRegs[HOLDING_REG_COUNT];
bool CoilBuff[COIL_COUNT];
bool DiscreteBuff[DISCRETE_COUNT];

void mb_init(void)
{
    uint16_t Count = 0;
    uint16_t device_info_data_size = sizeof(device_info.data);
    uint16_t Buff_Save_Size = CoilBuff_Save_Size + DiscreteBuff_Save_Size + HoldingRes_Save_Size*2 + InputRegs_Save_Size*2;

    // MODbus protocol is RTU
    // mb_protocol = MB_RTU;

    // MODbus protocol is TCP
    // mb_protocol = MB_TCP;

    // MODbus protocol is ASCII
    mb_protocol = MB_ASCII;

    // memset(CoilBuff, true, sizeof(CoilBuff));
    // memset(DiscreteBuff, true, sizeof(DiscreteBuff));
    // memset(HoldingRegs, 0xff, sizeof(HoldingRegs));
    // memset(InputRegs, 0xff, sizeof(InputRegs));

    // Read the device status from flash
    if(device_info_data_size>=Buff_Save_Size)
    {
        memcpy((uint8_t*)CoilBuff, (uint8_t*)&device_info.data[Count], CoilBuff_Save_Size);
        Count += CoilBuff_Save_Size;
        memcpy((uint8_t*)DiscreteBuff, (uint8_t*)&device_info.data[Count], DiscreteBuff_Save_Size);
        Count += DiscreteBuff_Save_Size;
        memcpy((uint8_t*)HoldingRegs, (uint8_t*)&device_info.data[Count], HoldingRes_Save_Size*2); 
        Count += HoldingRes_Save_Size*2;
        memcpy((uint8_t*)InputRegs, (uint8_t*)&device_info.data[Count], InputRegs_Save_Size*2);
        Count += InputRegs_Save_Size*2;

        LOG_MBRTU("Device status restored bytes:%d\r\n", Count);
    }
    else
    {
        LOG_MBRTU("Error: Data size is not enough to restore the device status!");
    }

    
}


