/*
 * @Author: LQS
 * @Date: 2025-05-05 22:38:47
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-06 11:04:02
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\FreeModbus\App\mb_func_callback.h
 * @Description: 
 */
#ifndef __MB_FUNC_CALLBACK_H_
#define __MB_FUNC_CALLBACK_H_

#include "main.h"
#include "mb.h"

#define INPUT_REG_COUNT 	50
#define HOLDING_REG_COUNT 	50
#define COIL_COUNT 			128
#define DISCRETE_COUNT 		32

extern SemaphoreHandle_t xRegMutex;

extern  uint16_t usRegInputBuf[INPUT_REG_COUNT];
extern  uint16_t usRegHoldingBuf[HOLDING_REG_COUNT];
extern  uint8_t	ucCoilBuf[COIL_COUNT / 8];
extern  uint8_t	ucDiscreteBuf[DISCRETE_COUNT / 8];

void MX_Modbus_Init(void);
void InitInputRegisters(void);
void InitHoldingRegisters(void);
void InitCoils(void);
void InitDiscreteInputs(void);

#endif


