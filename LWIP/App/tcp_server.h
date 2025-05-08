/*
 * @Author: LQS
 * @Date: 2025-04-18 16:19:18
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-25 20:43:28
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\LWIP\App\tcp_server.h
 * @Description:
 */
#ifndef __TCP_SERVER_H
#define __TCP_SERVER_H
#include "main.h"
#include "us_freertos.h"
#include "tcp_client.h"

typedef union
{
    struct
    {
        uint8_t part1:8;
        uint8_t part2:8;
        uint8_t part3:8;
        uint8_t part4:8;
    }ip;
    ip_addr_t ip_addr;
}IP_ADDR_TypeDef;

typedef struct
{
    void *      rx_data;
    uint16_t    rx_len;
    void *      tx_data;
    uint16_t    tx_len;
    uint16_t    client_port;
    IP_ADDR_TypeDef   client_ip;
}TCP_SERVER_DATA_TypeDef;
extern TCP_SERVER_DATA_TypeDef tcp_server_data;

// 状态机状态枚举
typedef enum 
{
    SERVER_STATE_INIT,
    SERVER_STATE_BIND,
    SERVER_STATE_LISTEN,
    SERVER_STATE_ACCEPT,
    SERVER_STATE_ERROR
} TCP_SERVER_STATE_TypeDef;
extern TCP_SERVER_STATE_TypeDef tcp_server_state;

void Tcp_S_Client_Task(void const * argument);

#endif
