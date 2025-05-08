/*
 * @Author: LQS
 * @Date: 2025-05-07 20:30:14
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-08 00:33:09
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\OTHER\mb_ascii_lrc.h
 * @Description: 
 */
#ifndef __MB_ASCII_LRC_H_
#define __MB_ASCII_LRC_H_
#include <stdint.h>
#include <stddef.h>

void byte_to_ascii(uint8_t byte, uint8_t* ascii);
uint8_t ascii_to_byte(const uint8_t* ascii);
uint8_t modbus_ascii_lrc_calc(const uint8_t* ascii_msg, size_t ascii_len);
uint8_t modbus_ascii_lrc_verify(const uint8_t* ascii_msg, size_t ascii_len);

#endif




 

