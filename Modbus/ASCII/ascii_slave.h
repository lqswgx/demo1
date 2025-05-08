/*
 * @Author: LQS
 * @Date: 2025-05-07 20:45:43
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 23:44:10
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\ASCII\ascii_slave.h
 * @Description: 
 */
#ifndef __ASCII_SLAVE_H_
#define __ASCII_SLAVE_H_

#include "main.h"

typedef enum
{ 
    ASCII_SLAVE_STATE_READY = 0, 
    ASCII_SLAVE_STATE_RX, 
    ASCII_SLAVE_STATE_HANDLE, 
    ASCII_SLAVE_STATE_TX, 
    ASCII_SLAVE_STATE_TIMEOUT, 
} ASCII_SlaveStateTypeDef; 
extern ASCII_SlaveStateTypeDef ascii_slave_state;

typedef struct
{ 
    uint8_t rx_reflag; 
    uint16_t rx_count; 
    uint8_t tx_completed; 
    uint16_t tx_count; 
    uint8_t error_flag; 
    uint8_t error_code; 
    uint8_t error_cmd; 
} ASCII_SlaveHandleTypeDef; 
extern ASCII_SlaveHandleTypeDef ascii_slave;

#define ASCII_RX_BUFF_LEN_MAX 512
#define ASCII_TX_BUFF_LEN_MAX 1024

extern uint8_t ascii_slave_rx_buff[ASCII_RX_BUFF_LEN_MAX]; 
extern uint8_t ascii_slave_tx_buff[ASCII_TX_BUFF_LEN_MAX];

void ascii_slave_init(void); 
void ascii_slave_poll(void);

void SlaveAsciiRxIRQHandler(void); 
void SlaveAsciiTxIRQHandler(void);


#endif




 

