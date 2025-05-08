/*
 * @Author: LQS
 * @Date: 2025-04-25 20:52:32
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-27 13:01:11
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\LWIP\App\udp_server.h
 * @Description: 
 */

#ifndef __UDP_SERVER_H_
#define __UDP_SERVER_H_
#include "main.h"
#include "us_freertos.h"
#include "tcp_client.h"

// 客户端信息结构体
typedef struct 
{
    ip_addr_t   client_ip;          // 客户端 IP
    uint16_t    client_port;        // 客户端端口
    TickType_t  last_active_time;   // 最后活跃时间
    bool        active_status;      // 是否有效  
} UDP_SERVER_INFO_TypeDef;

// 服务器端信息结构体
typedef struct
{
    void *      rx_data;
    uint16_t    rx_len;
    void *      tx_data;
    uint16_t    tx_len;
    uint16_t    client_port;
    ip_addr_t   client_ip;
}UDP_SERVER_DATA_TypeDef;
extern UDP_SERVER_DATA_TypeDef udp_server_data;

// 状态机状态枚举
typedef enum 
{
    UDP_SERVER_INIT,
    UDP_SERVER_BIND,
    UDP_SERVER_RECEIVE,
    UDP_SERVER_SEND,
    UDP_SERVER_ERROR
} UDP_SERVER_STATE_TypeDef;
extern UDP_SERVER_STATE_TypeDef udp_server_state;

void Udp_S_Client_Table_Init(void);
bool Udp_S_Client_Table_Update(ip_addr_t *addr, u16_t port);
void Udp_S_Client_Table_Cleanup(void);
void Udp_S_Client_Table_Print(void);

#endif

