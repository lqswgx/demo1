/*
 * @Author: LQS
 * @Date: 2025-05-06 21:47:56
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 18:52:43
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\OTHER\mb_bits.c
 * @Description: 
 */
#include "mb_bits.h"

void bool_array_to_uint8(const bool* bools, uint16_t bool_count, uint8_t* output, uint16_t *output_count)
{
    *output_count = (bool_count + 7) >> 3;
    for (uint16_t i = 0; i < *output_count; i++)
    {
        uint8_t value = 0;
        for (uint16_t j = 0; j < 8 && (i * 8 + j) < bool_count; j++)
        {
            value |= (bools[i * 8 + j] ? 1 : 0) << j;
        }
        output[i] = value;
    }
}

void uint8_to_bool_array(const uint8_t* input, uint16_t input_count, bool* bools, uint16_t *bool_count)
{
    *bool_count = input_count * 8;
    for (uint16_t i = 0; i < input_count; i++)
    {
        for (uint16_t j = 0; j < 8 && (i * 8 + j) < *bool_count; j++)
        {
            bools[i * 8 + j] = (input[i] >> j) & 1;
        }
    }
}












