/*
 * @Author: LQS
 * @Date: 2025-04-25 20:52:47
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-26 22:36:44
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\LWIP\App\udp_client.c
 * @Description: 
 */

#include "udp_client.h"
#include "lwip/api.h"

UDP_CLIENT_DATA_TypeDef UDP_client_data;
UDP_CLIENT_STATE_TypeDef udp_client_state = UDP_CLIENT_INIT;
extern struct netif gnetif;

// UDP 客户端任务
void Udp_Client_Task(void const * argument)
{
    struct netconn *conn = NULL;
    struct netbuf *tx_buf = NULL;
    struct netbuf *rx_buf = NULL;
    ip_addr_t client_addr;
    ip_addr_t server_addr;    
    err_t err;
	TickType_t last_link_check = 0;
	BaseType_t last_link_state = pdFALSE;
	BaseType_t current_link_state	=	pdFALSE;
	const TickType_t link_check_interval = 500;

    // 初始化服务器IP地址
    IP4_ADDR(&server_addr, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3, REMOTE_IP_ADDR4);
    // 初始化客户端IP地址
    IP4_ADDR(&client_addr, LOCAL_IP_ADDRESS1, LOCAL_IP_ADDRESS2, LOCAL_IP_ADDRESS3, LOCAL_IP_ADDRESS4);    

    // 初始化数据(测试用)
    UDP_client_data.tx_data  = "Hello, world!";
    UDP_client_data.tx_len   = strlen(UDP_client_data.tx_data);

    while (1) 
    {
        switch (udp_client_state) 
        {
            case UDP_CLIENT_INIT:
                LOG_UDP_CLIENT("UDP client init\n");
                if(conn != NULL)
                {
                    netconn_close(conn);
                    netconn_delete(conn);
                    conn = NULL;
                    LOG_UDP_CLIENT("Freed existing conn\n");
                }

                conn = netconn_new(NETCONN_UDP);
                if (conn == NULL)
                {
                    LOG_UDP_CLIENT("UDP client netconn_new failed\n");
                    osDelay(1000);
                    udp_client_state = UDP_CLIENT_ERROR;
                }
                else
                {
                    udp_client_state = UDP_CLIENT_BIND;
                }
                break;

            case UDP_CLIENT_BIND:
                LOG_UDP_CLIENT("UDP client bind\n");
                err = netconn_bind(conn, &client_addr, LOCAL_PORT);
                if (err != ERR_OK)
                {
                    LOG_UDP_CLIENT("UDP netconn_bind failed: %d\n", err);
                    osDelay(1000);
                    udp_client_state = UDP_CLIENT_ERROR;
                }
                else
                {
                    udp_client_state = UDP_CLIENT_SEND;
                }                
                break;

            case UDP_CLIENT_SEND:
                LOG_UDP_CLIENT("Sending data to server\n");
                tx_buf = netbuf_new();
                if (tx_buf == NULL)
                {
                    LOG_UDP_CLIENT("UDP client netbuf_new failed\n");
                    udp_client_state = UDP_CLIENT_ERROR;
                    break;
                }
                if (netbuf_ref(tx_buf, UDP_client_data.tx_data, UDP_client_data.tx_len) != ERR_OK)
                {
                    LOG_UDP_CLIENT("UDP client netbuf_ref failed\n");
                    udp_client_state = UDP_CLIENT_ERROR;
                    break;
                }
                
                err = netconn_sendto(conn, tx_buf, &server_addr, REMOTE_PORT);
                netbuf_delete(tx_buf);
                tx_buf = NULL;
                if (err == ERR_OK)
                {
                    LOG_UDP_CLIENT("UDP Client sent: %s\n", (char*)UDP_client_data.tx_data);
                    netconn_set_recvtimeout(conn, 5000);
                    udp_client_state = UDP_CLIENT_RECEIVE;
                } 
                else
                {
                    LOG_UDP_CLIENT("UDP client netconn_sendto failed: %d\n", err);
                    udp_client_state = UDP_CLIENT_ERROR;
                }
                break;

            case UDP_CLIENT_RECEIVE:
                LOG_UDP_CLIENT("Waiting for server response\n");
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
                            udp_client_state = UDP_CLIENT_ERROR;
                            continue;
                        }
                    }
                }

                err = netconn_recv(conn, &rx_buf);
                if (err == ERR_OK)
                {
                    netbuf_data(rx_buf, (void **)&UDP_client_data.rx_data, &UDP_client_data.rx_len);
                    LOG_UDP_CLIENT("UDP Client received %d bytes: %.*s\n", UDP_client_data.rx_len, UDP_client_data.rx_len, (char*)UDP_client_data.rx_data);
                    netbuf_delete(rx_buf);
                    rx_buf = NULL;
                    udp_client_state = UDP_CLIENT_SEND;
                } 
                else if (err == ERR_TIMEOUT) 
                {
					if(!IsLinkUp())
					{
						LOG_UDP_CLIENT("Link down, detected\n");
						udp_client_state = UDP_CLIENT_ERROR;
					}
                } 
                else 
                {
                    LOG_UDP_CLIENT("UDP client netconn_recv failed: %d\n", err);
                    udp_client_state = UDP_CLIENT_ERROR;
                }
                break;

            case UDP_CLIENT_ERROR:
                LOG_UDP_CLIENT("UDP client error, restarting\n");

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
                udp_client_state = UDP_CLIENT_INIT;
                osDelay(1000);
                break;

            default:
                udp_client_state = UDP_CLIENT_INIT;
                break;
        }

        osDelay(1000);
    }
}
