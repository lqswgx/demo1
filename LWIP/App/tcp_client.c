/*
 * @Author: LQS
 * @Date: 2025-04-18 16:28:14
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 23:24:05
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\LWIP\App\tcp_client.c
 * @Description: 
 */

#include "tcp_client.h"
#include "lwip.h"
#include "lwip/api.h"
#include "lwip/stats.h"
#include "lwip/netif.h"

#include "tcp_slave.h"

TCP_CLIENT_DATA_TypeDef tcp_client_data;
TCP_CLIENT_STATE_TypeDef tcp_client_state = CLIENT_STATE_INIT;

extern struct netif gnetif;
/**
 * @brief TCP客户端任务函数，负责管理与远程服务器的TCP连接、数据接收和发送。
 * 
 * 该函数通过状态机管理TCP客户端的不同状态，包括初始化、连接、接收数据、发送数据和断开连接。
 * 任务会定期检查网络连接状态，并在连接断开时自动重连。
 * 
 * @param argument 任务参数，未使用。
 * @return 无返回值。
 */
void Tcp_Client_Task(void const * argument)
{
	struct netconn *conn = NULL;  // TCP连接句柄
	err_t err;  // 错误码
	ip_addr_t server_ip;  // 服务器IP地址
	struct netbuf *buf;  // 网络数据缓冲区
	TickType_t last_link_check = 0;  // 上次检查网络连接状态的时间
	BaseType_t last_link_state = pdFALSE;  // 上次网络连接状态
	BaseType_t current_link_state = pdFALSE;  // 当前网络连接状态
	const TickType_t link_check_interval = 500;  // 网络连接状态检查间隔

	// 初始化服务器IP地址
	IP4_ADDR(&server_ip, REMOTE_IP_ADDR1, REMOTE_IP_ADDR2, REMOTE_IP_ADDR3, REMOTE_IP_ADDR4);
	
	while(1)
	{
		// 根据当前状态执行相应的操作
		switch (tcp_client_state) 
		{
			case CLIENT_STATE_INIT:
				// 初始化状态：释放现有连接并创建新的TCP连接
				LOG_TCP_CLIENT("TCPClientTask: Initializing\n");
				if (conn != NULL) 
				{
						netconn_close(conn);
						netconn_delete(conn);
						conn = NULL;
						LOG_TCP_CLIENT("TCPClientTask: Freed existing conn\n");
				}

				// 创建新的TCP连接
				conn = netconn_new(NETCONN_TCP);
				if (conn == NULL) 
				{
						LOG_TCP_CLIENT("TCPClientTask: Failed to create conn\n");
						osDelay(1000);
				} 
				else 
				{
						tcp_client_state = CLIENT_STATE_CONNECT;
				}
				break;

			case CLIENT_STATE_CONNECT:
				// 连接状态：尝试连接到服务器
				LOG_TCP_CLIENT("TCPClientTask: Connecting to %s:%d\n", ipaddr_ntoa(&server_ip), REMOTE_PORT);
				err = netconn_connect(conn, &server_ip, REMOTE_PORT);
				if (err == ERR_OK) 
				{ 
						LOG_TCP_CLIENT("TCPClientTask: Connected\n");
						last_link_check = xTaskGetTickCount();
						netconn_set_recvtimeout(conn, 5000);
						tcp_client_state = CLIENT_STATE_RECEIVE;
				}
				else
				{
						LOG_TCP_CLIENT("TCPClientTask: Connect failed: %d\n", err);
						tcp_client_state = CLIENT_STATE_DISCONNECT;
				}
				break;

			case CLIENT_STATE_RECEIVE:
				// 接收状态：等待接收数据并检查网络连接状态
				LOG_TCP_CLIENT("TCPClientTask: Waiting to receive\n");

				// 定期检查网络连接状态
				if(xTaskGetTickCount() - last_link_check >= link_check_interval)
				{
						current_link_state = IsLinkUp();
						if(current_link_state != last_link_state)
						{
								LOG_TCP_CLIENT("TCPClientTask: Link state changed: %s\n", current_link_state ? "UP" : "DOWN");
								last_link_state = current_link_state;
								if(current_link_state)netif_set_link_up(&gnetif);
								else netif_set_link_down(&gnetif);
								if(!current_link_state)
								{
										LOG_TCP_CLIENT("TCPClientTask: Link down, detected\n");
										tcp_client_state = CLIENT_STATE_DISCONNECT;
										continue;
								}
						}
				}
				
				// 接收数据
				err = netconn_recv(conn, &buf);
				if (err == ERR_OK) 
				{
						// 处理接收到的数据
						do 
						{
							netbuf_data(buf, &tcp_client_data.rx_data, &tcp_client_data.rx_len);
							tcp_slave.rx_count = tcp_client_data.rx_len;
							memcpy((uint8_t*)tcp_slave_rx_buff, (uint8_t*)tcp_client_data.rx_data, tcp_client_data.rx_len);
							LOG_TCP_CLIENT("TCPClientTask: Received: %.*s\n", tcp_slave.rx_count, (char *)tcp_slave_rx_buff);
							SlaveTcpRxHandler();
						} while (netbuf_next(buf) >= 0);

						if(tcp_slave.rx_reflag)
						{
							netbuf_delete(buf);
							tcp_client_state = CLIENT_STATE_SEND;
						}
				} 
				else if (err == ERR_TIMEOUT) 
				{
						// 处理接收超时
						if(!IsLinkUp())
						{
							LOG_TCP_CLIENT("TCPClientTask: Link down, detected\n");
							tcp_client_state = CLIENT_STATE_DISCONNECT;
						}
				}  
				else 
				{
						// 处理接收失败
						LOG_TCP_CLIENT("TCPClientTask: Recv failed: %d\n", err);
						tcp_client_state = CLIENT_STATE_DISCONNECT;
				}
				break;
			
			case CLIENT_STATE_SEND:
				// 发送状态：发送接收到的数据
				LOG_TCP_CLIENT("TCPClientTask: Sending\n");
				if(xSemaphoreTake(tcp_tx_sem, 0) == pdTRUE)
				{
					err = netconn_write(conn, (char *)tcp_slave_tx_buff, tcp_slave.tx_count, NETCONN_COPY);
					tcp_slave.tx_completed = 1; // 发送完成
					if (err == ERR_OK)
					{
							LOG_TCP_CLIENT("TCPClientTask: Sent: %.*s\n", tcp_slave.tx_count, (char *)tcp_slave_tx_buff);
							tcp_client_state = CLIENT_STATE_RECEIVE;
					} 
					else 
					{
							LOG_TCP_CLIENT("TCPClientTask: Send failed: %d\n", err);
							tcp_client_state = CLIENT_STATE_DISCONNECT;
					}
				}
				break;

			case CLIENT_STATE_DISCONNECT:
				// 断开连接状态：关闭并释放连接
				LOG_TCP_CLIENT("TCPClientTask: Disconnecting\n");
				if(conn != NULL)
				{
						netconn_close(conn);
						netconn_delete(conn);
						conn = NULL;
				}
				// PrintLwIPStatus();
				osDelay(1000);
				tcp_client_state = CLIENT_STATE_INIT;
				break;

             case CLIENT_STATE_ERROR:
				// 错误状态：处理错误情况
				LOG_TCP_CLIENT("TCPClientTask: Error state\n");
				tcp_client_state = CLIENT_STATE_DISCONNECT;
				break;

			default:
				tcp_client_state = CLIENT_STATE_ERROR;
				break;
	}

		// 任务延时
		osDelay(10);
	}
}

/**
 * @brief 检查网络链接状态
 * 
 * 该函数通过读取PHY寄存器的状态来判断网络链接是否已建立。
 * 
 * @return BaseType_t 
 *         - pdTRUE: 网络链接已建立
 *         - pdFALSE: 网络链接未建立或读取寄存器失败
 */
BaseType_t IsLinkUp(void)
{
    uint32_t status = 0;

    // 读取PHY寄存器的状态
    if (HAL_ETH_ReadPHYRegister(&heth, LAN8720A_PHY_ADDRESS, 1, &status) == HAL_OK) 
    {
        // 检查状态寄存器的第2位（Link Status位）是否为1
        return (status & 0x04) ? pdTRUE : pdFALSE;
    }
    else
    {
        // 如果读取寄存器失败，输出错误信息
        printf("IsLinkUp: Failed to read PHY register\n");
    }

    // 默认返回链接未建立
    return pdFALSE;
}

/**
 * @brief 打印LwIP的状态信息
 * 
 * 该函数用于打印LwIP库的当前状态信息，包括内存使用情况、TCP PCB（协议控制块）使用情况
 * 以及Netconn使用情况。这些信息仅在启用了LWIP_STATS宏时才会被打印，否则会提示状态信息未启用。
 * 
 * @param 无
 * @return 无
 */
void PrintLwIPStatus(void)
{
#if LWIP_STATS
    // 打印内存使用情况：已使用内存、最大内存、可用内存
    printf("MEM used/max/avail: %u/%u/%u\n", lwip_stats.mem.used, lwip_stats.mem.max, lwip_stats.mem.avail);
    
    // 打印TCP PCB使用情况：已使用、最大、可用
    printf("TCP PCB used/max/avail: %u/%u/%u\n",lwip_stats.memp[MEMP_NUM_TCP_PCB]->used, lwip_stats.memp[MEMP_NUM_TCP_PCB]->max, lwip_stats.memp[MEMP_NUM_TCP_PCB]->avail);
    
    // 打印Netconn使用情况：已使用、最大、可用
    printf("Netconn used/max/avail: %u/%u/%u\n",lwip_stats.memp[MEMP_NUM_NETCONN]->used, lwip_stats.memp[MEMP_NUM_NETCONN]->max, lwip_stats.memp[MEMP_NUM_NETCONN]->avail);
#else
    // 如果未启用LWIP_STATS宏，则提示状态信息未启用
    printf("LwIP stats not enabled\n");
#endif
}
