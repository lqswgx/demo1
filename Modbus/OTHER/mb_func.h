/*
 * @Author: LQS
 * @Date: 2025-05-07 18:25:39
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 19:43:12
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\OTHER\mb_func.h
 * @Description: 
 */
#ifndef __MB_FUNC_H_
#define __MB_FUNC_H_
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint8_t (*ModbusFuncHandler)(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);
extern ModbusFuncHandler modbus_handlers[17];

uint8_t mb_rx_check_handle(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);

uint8_t mb_error_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);
uint8_t mb_func_01_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);
uint8_t mb_func_02_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);
uint8_t mb_func_03_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);
uint8_t mb_func_04_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);
uint8_t mb_func_05_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);
uint8_t mb_func_06_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);
uint8_t mb_func_0F_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);
uint8_t mb_func_10_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len);


#endif



 

