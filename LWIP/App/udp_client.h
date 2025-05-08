/*
 * @Author: LQS
 * @Date: 2025-04-25 20:52:56
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-26 15:55:08
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\LWIP\App\udp_client.h
 * @Description: 
 */

#ifndef __UDP_CLIENT_H_
#define __UDP_CLIENT_H_
#include "main.h"
#include "us_freertos.h"
#include "tcp_client.h"

typedef struct
{
    void *      rx_data;
    uint16_t    rx_len;
    void *      tx_data;
    uint16_t    tx_len;
}UDP_CLIENT_DATA_TypeDef;
extern UDP_CLIENT_DATA_TypeDef UDP_client_data;

// 状态机状态
typedef enum {
    UDP_CLIENT_INIT,
    UDP_CLIENT_BIND,
    UDP_CLIENT_SEND,
    UDP_CLIENT_RECEIVE,
    UDP_CLIENT_ERROR
} UDP_CLIENT_STATE_TypeDef;
extern UDP_CLIENT_STATE_TypeDef udp_client_state;

#endif


 

