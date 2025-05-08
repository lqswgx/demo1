/*
 * @Author: LQS
 * @Date: 2025-05-06 21:47:56
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 18:53:55
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\OTHER\mb_bits.h
 * @Description: 
 */
#ifndef _MB_BITS_H
#define _MB_BITS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void bool_array_to_uint8(const bool* bools, uint16_t bool_count, uint8_t* output, uint16_t *output_count);
void uint8_to_bool_array(const uint8_t* input, uint16_t input_count, bool* bools, uint16_t *bool_count);

#endif















