/*
 * @Author: LQS
 * @Date: 2025-05-06 21:47:56
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 12:12:53
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\RTU\rtu_crc.h
 * @Description: 
 */
#ifndef _MB_RTU_CRC16_H
#define _MB_RTU_CRC16_H

#include  "main.h"

uint16_t CRC16(uint8_t *puchMsg, uint16_t usDataLen);

#endif
