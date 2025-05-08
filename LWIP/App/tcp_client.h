/*
 * @Author: LQS
 * @Date: 2025-04-18 16:28:22
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-23 18:29:30
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\LWIP\App\tcp_client.h
 * @Description: 
 */
#ifndef __TCP_CLIENT_H
#define __TCP_CLIENT_H
#include "main.h"
#include "us_freertos.h"

typedef struct
{
    void *      rx_data;
    uint16_t    rx_len;
    void *      tx_data;
    uint16_t    tx_len;
}TCP_CLIENT_DATA_TypeDef;
extern TCP_CLIENT_DATA_TypeDef tcp_client_data;

// 状态机状态枚举
typedef enum 
{
    CLIENT_STATE_INIT,
    CLIENT_STATE_CONNECT,
    CLIENT_STATE_RECEIVE,
    CLIENT_STATE_SEND,
    CLIENT_STATE_DISCONNECT,
    CLIENT_STATE_ERROR
} TCP_CLIENT_STATE_TypeDef;
extern TCP_CLIENT_STATE_TypeDef tcp_client_state;

BaseType_t IsLinkUp(void);
void ResetPHY(void);
BaseType_t InitPHY(void);
void PrintLwIPStatus(void);
#endif
