/*
 * @Author: LQS
 * @Date: 2025-04-25 20:52:22
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-27 13:18:00
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\LWIP\App\udp_server.c
 * @Description: 
 */

#include "udp_server.h"
#include "lwip/api.h"

// 配置参数
#define UDP_SERVER_PORT             8080    // UDP 监听端口
#define UDP_SERVER_MAX_CLIENTS      10      // 最大跟踪客户端数
#define UDP_CLIENT_TIMEOUT_TICKS    60000   // 客户端超时（60秒）
#define UDP_CLEANUP_INTERVAL_TICKS  30000   // 清理间隔（30秒）

// 全局变量
static UDP_SERVER_INFO_TypeDef udp_s_client_table[UDP_SERVER_MAX_CLIENTS]; // 客户端地址表
static osMutexId UDP_S_Client_MutexSemHandle; // 互斥量
static TickType_t last_cleanup_time; // 上次清理时间
extern struct netif gnetif;

UDP_SERVER_DATA_TypeDef udp_server_data; // UDP 服务器数据
UDP_SERVER_STATE_TypeDef udp_server_state = UDP_SERVER_INIT; // UDP 服务器状态

// UDP 服务器任务
void Udp_Server_Task(void const * argument)
{
    struct netconn *conn = NULL;
    struct netbuf *rx_buf = NULL;
    struct netbuf *tx_buf = NULL;
    ip_addr_t server_addr;
    err_t err;
	TickType_t last_link_check = 0;
	BaseType_t last_link_state = pdFALSE;
	BaseType_t current_link_state	=	pdFALSE;
	const TickType_t link_check_interval = 500;

    // 初始化客户端表
    Udp_S_Client_Table_Init();

    // 初始化服务器IP地址
    IP4_ADDR(&server_addr, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3, REMOTE_IP_ADDR4);

    while (1) 
    {
        // 定期清理不活跃客户端
        if (xTaskGetTickCount() - last_cleanup_time > UDP_CLEANUP_INTERVAL_TICKS)
        {
            Udp_S_Client_Table_Cleanup();
            Udp_S_Client_Table_Print();
            last_cleanup_time = xTaskGetTickCount();
        }

        switch (udp_server_state)
        {
            case UDP_SERVER_INIT:
                LOG_UDP_SERVER("UDP server init\n");
                if(conn != NULL)
                {
                    netconn_close(conn);
                    netconn_delete(conn);
                    conn = NULL;
                    LOG_UDP_SERVER("Freed existing conn\n");
                }
                conn = netconn_new(NETCONN_UDP);
                if (conn == NULL)
                {
                    LOG_UDP_SERVER("fail to create new conn\n");
                    osDelay(1000);
                    udp_server_state = UDP_SERVER_ERROR;
                }
                else
                {
                    udp_server_state = UDP_SERVER_BIND;
                }
                break;

            case UDP_SERVER_BIND:
                LOG_UDP_SERVER("UDP server bind\n");
                err = netconn_bind(conn, &server_addr, REMOTE_PORT);
                if (err != ERR_OK)
                {
                    LOG_UDP_SERVER("UDP netconn_bind failed: %d\n", err);
                    osDelay(1000);
                    udp_server_state = UDP_SERVER_ERROR;
                }
                else
                {
                    netconn_set_recvtimeout(conn, 5000);
                    udp_server_state = UDP_SERVER_RECEIVE;
                }                
                break;
                
            case UDP_SERVER_RECEIVE:
                LOG_UDP_SERVER("UDP server receive\n");
                if(xTaskGetTickCount() - last_link_check >= link_check_interval)
				{
					current_link_state = IsLinkUp();
					if(current_link_state != last_link_state)
					{
						LOG_UDP_SERVER("Link state changed: %s\n", current_link_state ? "UP" : "DOWN");
						last_link_state = current_link_state;
						if(current_link_state)netif_set_link_up(&gnetif);
						else netif_set_link_down(&gnetif);
						if(!current_link_state)
						{
							LOG_UDP_SERVER("Link down, detected\n");
							udp_server_state = UDP_SERVER_ERROR;
							continue;
						}
					}
				}

                err = netconn_recv(conn, &rx_buf);
                if (err == ERR_OK) 
                {
                    ip_addr_copy(udp_server_data.client_ip, rx_buf->addr);
                    udp_server_data.client_port = rx_buf->port;
                    netbuf_data(rx_buf, (void **)&udp_server_data.rx_data, &udp_server_data.rx_len);
                    LOG_UDP_SERVER("Received from %s:%d, %d bytes: %.*s\n",ipaddr_ntoa(&udp_server_data.client_ip), udp_server_data.client_port, udp_server_data.rx_len, udp_server_data.rx_len, (char *)udp_server_data.rx_data);
                    Udp_S_Client_Table_Update(&udp_server_data.client_ip, udp_server_data.client_port);
                    udp_server_state = UDP_SERVER_SEND;
                } 
                else if (err == ERR_TIMEOUT)
                {
					if(!IsLinkUp())
					{
						LOG_UDP_SERVER("Link down, detected\n");
						udp_server_state = UDP_SERVER_ERROR;
					}
                } 
                else 
                {
                    LOG_UDP_SERVER("UDP netconn_recvfrom failed: %d\n", err);
                    udp_server_state = UDP_SERVER_ERROR;
                }
                break;

            case UDP_SERVER_SEND:
                LOG_UDP_SERVER("UDP server send\n");
                if (rx_buf != NULL)
                {
                    tx_buf = netbuf_new();
                    if (tx_buf == NULL)
                    {
                        LOG_UDP_SERVER("UDP fail to create new tx_buf\n");
                        udp_server_state = UDP_SERVER_ERROR;
                        break;
                    }
                    if (netbuf_ref(tx_buf, udp_server_data.rx_data, udp_server_data.rx_len) != ERR_OK)
                    {
                        LOG_UDP_SERVER("UDP netbuf_ref failed\n");
                        udp_server_state = UDP_SERVER_ERROR;
                        break;
                    }
                    err = netconn_sendto(conn, tx_buf, &udp_server_data.client_ip, udp_server_data.client_port);
                    netbuf_delete(tx_buf);
                    netbuf_delete(rx_buf);
                    tx_buf = NULL;
                    rx_buf = NULL;


                    if (err == ERR_OK)
                    {
                        LOG_UDP_SERVER("Sent %d bytes to %s:%d\n", udp_server_data.rx_len, ipaddr_ntoa(&udp_server_data.client_ip), udp_server_data.client_port);
                        udp_server_state = UDP_SERVER_RECEIVE;
                    } 
                    else 
                    {
                        LOG_UDP_SERVER("UDP netconn_sendto failed: %d\n", err);
                        udp_server_state = UDP_SERVER_ERROR;
                    }
                }
                else 
                {
                    udp_server_state = UDP_SERVER_RECEIVE;
                }
                break;

            case UDP_SERVER_ERROR:
                LOG_UDP_SERVER("UDP server error\n");
                if (rx_buf != NULL)
                {
                    netbuf_delete(rx_buf);
                    rx_buf = NULL;
                }
                if (tx_buf != NULL)
                {
                    netbuf_delete(tx_buf);
                    tx_buf = NULL;
                }
                if (conn != NULL)
                {
                    netconn_delete(conn);
                    conn = NULL;
                }
                udp_server_state = UDP_SERVER_INIT;
                osDelay(1000);
                break;

            default:
                udp_server_state = UDP_SERVER_INIT;
                break;
        }

        osDelay(10);
    }
}

/**
 * @brief 初始化客户端表
 * @param 无
 * @return void
 */
void Udp_S_Client_Table_Init(void) 
{
    memset(udp_s_client_table, 0, sizeof(udp_s_client_table));
    osMutexDef(UDP_S_Client_MutexSem);
    UDP_S_Client_MutexSemHandle = osMutexCreate(osMutex(UDP_S_Client_MutexSem));
    if (UDP_S_Client_MutexSemHandle == NULL)
    {
        LOG_UDP_SERVER("Failed to create client table mutex\n");
    }
    last_cleanup_time = xTaskGetTickCount();
}

/**
 * @brief 更新客户端表
 * @param addr 客户端IP地址
 * @param port 客户端端口
 * @return bool 成功返回true，失败返回false
 */
bool Udp_S_Client_Table_Update(ip_addr_t *addr, u16_t port) 
{
    uint8_t i = 0;

    if (xSemaphoreTake(UDP_S_Client_MutexSemHandle, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        TickType_t current_time = xTaskGetTickCount();
        
        // 查找现有客户端
        for (i = 0; i < UDP_SERVER_MAX_CLIENTS; i++)
        {
            if (udp_s_client_table[i].active_status && ip_addr_cmp(&udp_s_client_table[i].client_ip, addr) && udp_s_client_table[i].client_port == port)
            {
                udp_s_client_table[i].last_active_time = current_time;
                LOG_UDP_SERVER("client is already in table\n");
                xSemaphoreGive(UDP_S_Client_MutexSemHandle);
                return pdTRUE;
            }
        }

        // 查找空槽
        for (i = 0; i < UDP_SERVER_MAX_CLIENTS; i++)
        {
            if (!udp_s_client_table[i].active_status)
            {
                ip_addr_copy(udp_s_client_table[i].client_ip, *addr);
                udp_s_client_table[i].client_port = port;
                udp_s_client_table[i].last_active_time = current_time;
                udp_s_client_table[i].active_status = true;
                LOG_UDP_SERVER("Added client %s:%d at slot %d\n", ipaddr_ntoa(addr), port, i);
                xSemaphoreGive(UDP_S_Client_MutexSemHandle);
                return pdTRUE;
            }
        }
        LOG_UDP_SERVER("Client table full, cannot add %s:%d\n", ipaddr_ntoa(addr), port);
        xSemaphoreGive(UDP_S_Client_MutexSemHandle);
        return pdFALSE;
    }
    LOG_UDP_SERVER("Failed to take client table mutex\n");
    return pdFALSE;
}

/**
 * @brief 清理不活跃客户端
 * @param 无
 * @return void
 */
void Udp_S_Client_Table_Cleanup(void)
{
    if (xSemaphoreTake(UDP_S_Client_MutexSemHandle, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        TickType_t current_time = xTaskGetTickCount();
        for (int i = 0; i < UDP_SERVER_MAX_CLIENTS; i++)
        {
            if (udp_s_client_table[i].active_status && (current_time - udp_s_client_table[i].last_active_time > UDP_CLIENT_TIMEOUT_TICKS))
            {
                LOG_UDP_SERVER("Removed inactive client %s:%d from slot %d\n",ipaddr_ntoa(&udp_s_client_table[i].client_ip), udp_s_client_table[i].client_port, i);
                udp_s_client_table[i].active_status = false;
            }
        }
        xSemaphoreGive(UDP_S_Client_MutexSemHandle);
    }
}


/**
 * @brief 打印客户端列表
 * @param 无
 * @return void
 */
void Udp_S_Client_Table_Print(void)
{
    if (xSemaphoreTake(UDP_S_Client_MutexSemHandle, pdMS_TO_TICKS(100)) == pdTRUE)
    {
        LOG_UDP_SERVER("Active clients:\n");
        uint8_t active_count = 0;
        for (uint8_t i = 0; i < UDP_SERVER_MAX_CLIENTS; i++)
        {
            if (udp_s_client_table[i].active_status)
            {
                LOG_UDP_SERVER("Slot %d: %s:%d, last active time %d ticks ago\n",i, ipaddr_ntoa(&udp_s_client_table[i].client_ip), udp_s_client_table[i].client_port,xTaskGetTickCount() - udp_s_client_table[i].last_active_time);
                active_count++;
            }
        }
        LOG_UDP_SERVER("Total active clients: %d\n", active_count);
        xSemaphoreGive(UDP_S_Client_MutexSemHandle);
    }
}
