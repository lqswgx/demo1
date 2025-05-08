/*
 * @Author: LQS
 * @Date: 2025-05-06 22:07:12
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 13:35:01
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\RTU\rtu_init.h
 * @Description: 
 */
#ifndef __MB_INIT_H_
#define __MB_INIT_H_

#include "main.h"

#include "mb_bits.h"
#include "mb_rtu_crc16.h"
#include "mb_ascii_lrc.h"
#include "mb_func.h"

#include "mb_coilsbuff_rw.h"
#include "mb_discretebuff_ro.h"
#include "mb_holdregs_rw.h"
#include "mb_inputregs_ro.h"

typedef enum
{
    MB_RTU,
    MB_TCP,
    MB_ASCII
}MB_PROTOCOL;
extern MB_PROTOCOL mb_protocol;

#define COIL_COUNT 			32
#define DISCRETE_COUNT 		32
#define HOLDING_REG_COUNT 	30
#define INPUT_REG_COUNT 	30

extern bool CoilBuff[COIL_COUNT];
extern bool DiscreteBuff[DISCRETE_COUNT];
extern uint16_t HoldingRegs[HOLDING_REG_COUNT];
extern uint16_t InputRegs[INPUT_REG_COUNT];

void mb_init(void);

#endif





 

