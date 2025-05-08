/*
 * @Author: LQS
 * @Date: 2025-05-07 18:22:19
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 23:25:43
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\TCP\tcp_slave.h
 * @Description: 
 */
#ifndef __TCP_SLAVE_H_
#define __TCP_SLAVE_H_

#include "main.h"
#include "us_freertos.h"

typedef enum
{
    TCP_SLAVE_STATE_READY = 0,
    TCP_SLAVE_STATE_RX,
    TCP_SLAVE_STATE_HANDLE,
    TCP_SLAVE_STATE_TX,
    TCP_SLAVE_STATE_TIMEOUT,
} TCP_SlaveStateTypeDef;
extern TCP_SlaveStateTypeDef tcp_slave_state;

typedef struct
{
    uint8_t rx_reflag;      // 接收完成标志
    uint16_t rx_count;      // 接收数据长度
    uint8_t tx_completed;   // 发送完成标志
    uint16_t tx_count;      // 发送数据长度
    uint16_t transaction_id; // Modbus TCP 事务 ID
    uint8_t unit_id;        // 单元 ID
} TCP_SlaveHandleTypeDef;
extern TCP_SlaveHandleTypeDef tcp_slave;

#define TCP_TX_BUFF_LEN_MAX 256
#define TCP_RX_BUFF_LEN_MAX 1024

extern uint8_t tcp_slave_rx_buff[TCP_RX_BUFF_LEN_MAX];
extern uint8_t tcp_slave_tx_buff[TCP_TX_BUFF_LEN_MAX];

extern SemaphoreHandle_t tcp_tx_sem; // 发送信号量

void tcp_slave_init(void);
void tcp_slave_poll(void);

void SlaveTcpRxHandler(void);
void SlaveTcpTxHandler(void);



#endif





 

