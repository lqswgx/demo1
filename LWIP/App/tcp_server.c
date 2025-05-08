/*
 * @Author: LQS
 * @Date: 2025-04-18 16:19:34
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-25 23:49:59
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\LWIP\App\tcp_server.c
 * @Description: 
 */

#include "tcp_server.h"
#include "lwip/api.h"

#define CLIENTS_SIZE 5  // 最大客户端数量

TCP_SERVER_DATA_TypeDef tcp_server_data;
TCP_SERVER_STATE_TypeDef tcp_server_state	=	SERVER_STATE_INIT;
extern struct netif gnetif;

uint8_t active_clients = 0;
TCP_SERVER_DATA_TypeDef tcp_server_client_data[CLIENTS_SIZE];

/**
 * @brief TCP服务器任务函数，负责初始化、绑定、监听和接受客户端连接。
 * 
 * 该函数是一个无限循环任务，根据`tcp_server_state`的状态机执行不同的操作，
 * 包括初始化服务器、绑定IP和端口、监听连接、接受客户端连接以及处理错误状态。
 * 
 * @param argument 任务参数，未使用。
 * @return 无返回值。
 */
void Tcp_Server_Task(void const * argument)
{
    struct netconn *server_conn = NULL;  // 服务器连接结构体
    struct netconn *client_conn = NULL;  // 客户端连接结构体
    ip_addr_t server_ip;                 // 服务器IP地址
    err_t err;                           // 错误码
    osThreadId client_handle = NULL;     // 客户端任务句柄

    // 初始化服务器IP地址
    IP4_ADDR(&server_ip, LOCAL_IP_ADDRESS1, LOCAL_IP_ADDRESS2, LOCAL_IP_ADDRESS3, LOCAL_IP_ADDRESS4);

    while (1)
    {
        switch (tcp_server_state)
        {
            case SERVER_STATE_INIT:
                // 初始化服务器连接
                LOG_TCP_SERVER("TCPServerTask: Initializing\n");
                if(server_conn != NULL)
                {
                    // 释放已存在的服务器连接
                    netconn_close(server_conn);
                    netconn_delete(server_conn);
                    server_conn = NULL;
                    LOG_TCP_SERVER("TCPServerTask: Freed existing conn\n");
                }
                // 创建新的TCP服务器连接
                server_conn = netconn_new(NETCONN_TCP);
                if (server_conn == NULL)
                {
                    LOG_TCP_SERVER("TCPServerTask: Failed to create server conn\n");
                    osDelay(1000);
                    tcp_server_state = SERVER_STATE_ERROR;
                }
                else
                {
                    tcp_server_state = SERVER_STATE_BIND;
                }
                break;

            case SERVER_STATE_BIND:
                // 绑定服务器到指定IP和端口
                LOG_TCP_SERVER("TCPServerTask: Binding to IP %s:%d\n", ipaddr_ntoa(&server_ip), LOCAL_PORT);
                err = netconn_bind(server_conn, &server_ip, LOCAL_PORT);
                if(err != ERR_OK)
                {
                    LOG_TCP_SERVER("TCPServerTask: Bind failed: %d\n", err);
                    osDelay(1000);
                    tcp_server_state = SERVER_STATE_ERROR;
                }
                else
                {
                    tcp_server_state = SERVER_STATE_LISTEN;
                }
                break;  

            case SERVER_STATE_LISTEN:
                // 开始监听客户端连接
                LOG_TCP_SERVER("TCPServerTask: Listening for connections\n");
                err = netconn_listen(server_conn);
                if(err != ERR_OK)
                {
                    LOG_TCP_SERVER("TCPServerTask: Listen failed: %d\n", err);
                    tcp_server_state = SERVER_STATE_ERROR;
                }
                else
                {
                    // 设置接收超时时间
                    netconn_set_recvtimeout(server_conn, 5000);
                    tcp_server_state = SERVER_STATE_ACCEPT;
                }
                break;
            
            case SERVER_STATE_ACCEPT:
                // 接受客户端连接
                LOG_TCP_SERVER("TCPServerTask: Accepting connections\n");
                err = netconn_accept(server_conn, &client_conn);
                if(err == ERR_OK)
                {
                    if(active_clients < CLIENTS_SIZE)
                    {
                        // 创建客户端任务处理连接
                        osThreadDef(TCP_S_CLIENT_TASK, Tcp_S_Client_Task, osPriorityLow, 0, 512);
                        client_handle = osThreadCreate(osThread(TCP_S_CLIENT_TASK), client_conn);
                        if(client_handle == NULL)
                        {
                            LOG_TCP_SERVER("TCPServerTask: Failed to create client task\n");
                            netconn_close(client_conn);
                            netconn_delete(client_conn);
                            client_conn = NULL;
                        }
                        else
                        {
                            LOG_TCP_SERVER("TCPServerTask: Client task created\n");
                        }
                    }
                    else
                    {
                        // 达到最大客户端数量，关闭连接
                        LOG_TCP_SERVER("TCPServerTask: Max clients reached: %d\n", CLIENTS_SIZE);
                        netconn_close(client_conn);
                        netconn_delete(client_conn);
                        client_conn = NULL;
                    }
                    tcp_server_state = SERVER_STATE_ACCEPT;
                }
                else if(err == ERR_TIMEOUT)
                {
                    // 超时后继续等待连接
                    tcp_server_state = SERVER_STATE_ACCEPT;
                }
                else
                {
                    LOG_TCP_SERVER("TCPServerTask: Accept failed: %d\n", err);
                    tcp_server_state = SERVER_STATE_ERROR;
                }
                break;
            
            case SERVER_STATE_ERROR:
                // 处理错误状态，释放资源并重新初始化
                LOG_TCP_SERVER("TCPServerTask: Error state\n");
                if(client_conn != NULL)
                {
                    netconn_close(client_conn);
                    netconn_delete(client_conn);
                    client_conn = NULL;
                }
                if(server_conn != NULL)
                {
                    netconn_close(server_conn);
                    netconn_delete(server_conn);
                    server_conn = NULL;
                }
                PrintLwIPStatus();
                osDelay(1000);
                tcp_server_state = SERVER_STATE_INIT;
                break; 
            
            default:
                tcp_server_state = SERVER_STATE_ERROR;
                break;
        }

        // 任务延时
        osDelay(10);
    }
}


void Tcp_S_Client_Task(void const * pvParameters)
{
	struct netconn * client_conn = (struct netconn *)pvParameters;
	struct netbuf * buf = NULL;
	TCP_CLIENT_STATE_TypeDef state = CLIENT_STATE_RECEIVE;
	err_t err;
	TickType_t last_link_check = 0;
	BaseType_t last_link_state = pdFALSE;
	BaseType_t current_link_state	=	pdFALSE;
	const TickType_t link_check_interval = 500;
	uint8_t index = 0;

	LOG_TCP_SERVER("TCP_S_ClientTask: Client task started\n");
	LOG_TCP_SERVER("TCP_S_ClientTask: active clents: %d\n", ++active_clients);
    index = active_clients-1;
    netconn_getaddr(client_conn, &tcp_server_client_data[index].client_ip.ip_addr, &tcp_server_client_data[index].client_port, 0);
    LOG_TCP_SERVER("TCP_S_ClientTask: Client IP: %s:%d\n",ip4addr_ntoa(&tcp_server_client_data[index].client_ip.ip_addr),tcp_server_client_data[index].client_port);

	netconn_set_recvtimeout(client_conn, 5000);
	
	while(1)
	{
		switch(state)
		{
			case CLIENT_STATE_INIT:
				
				break;
			
			case CLIENT_STATE_CONNECT:
				
				break;
			
			case CLIENT_STATE_RECEIVE:
				LOG_TCP_SERVER("TCP_S_ClientTask: Receiving data\n");
				if(xTaskGetTickCount() - last_link_check >= link_check_interval)
				{
					current_link_state = IsLinkUp();
					if(current_link_state != last_link_state)
					{
						LOG_TCP_SERVER("TCP_S_ClientTask: Link state changed: %s\n", current_link_state ? "UP" : "DOWN");
						last_link_state = current_link_state;
						if(current_link_state)netif_set_link_up(&gnetif);
						else netif_set_link_down(&gnetif);
						if(!current_link_state)
						{
							LOG_TCP_SERVER("TCP_S_ClientTask: Link down, detected\n");
							state = CLIENT_STATE_ERROR;
							continue;
						}
					}
				}
				err = netconn_recv(client_conn, &buf);
				if (err == ERR_OK) 
				{
					do 
					{
                        tcp_server_client_data[index].rx_len = 0;
						netbuf_data(buf, &tcp_server_client_data[index].rx_data, &tcp_server_client_data[index].rx_len);
						LOG_TCP_SERVER("TCP_S_ClientTask: Received: %.*s\n", tcp_server_client_data[index].rx_len, (char *)tcp_server_client_data[index].rx_data);
					} while (netbuf_next(buf) >= 0);
					netbuf_delete(buf);
					state = CLIENT_STATE_SEND;
				} 
				else if (err == ERR_TIMEOUT) 
				{
					if(!IsLinkUp())
					{
						LOG_TCP_SERVER("TCP_S_ClientTask: Link down, detected\n");
						state = CLIENT_STATE_ERROR;
					}
				}  
				else 
				{
					netbuf_delete(buf);
					LOG_TCP_SERVER("TCP_S_ClientTask: Recv failed: %d\n", err);
					state = CLIENT_STATE_DISCONNECT;
				} 
				break;

			case CLIENT_STATE_SEND:
				LOG_TCP_SERVER("TCP_S_ClientTask: Sending data\n");
				err = netconn_write(client_conn, tcp_server_client_data[index].rx_data, tcp_server_client_data[index].rx_len, NETCONN_COPY);
				if (err != ERR_OK)
				{
					LOG_TCP_SERVER("TCP_S_ClientTask: Send failed: %d\n", err);
					state = CLIENT_STATE_DISCONNECT;
				}
				else
				{
					LOG_TCP_SERVER("TCP_S_ClientTask: Sent: %.*s\n", tcp_server_client_data[index].rx_len, (char *)tcp_server_client_data[index].rx_data);
					state = CLIENT_STATE_RECEIVE;
				}	
				break;

			case CLIENT_STATE_DISCONNECT:
				LOG_TCP_SERVER("TCP_S_ClientTask: Disconnecting\n");
				if (client_conn != NULL) 
				{
					netconn_close(client_conn);
					netconn_delete(client_conn);
					client_conn = NULL;
				}
				LOG_TCP_SERVER("TCP_S_ClientTask: Active clients: %d\n", --active_clients);
				osThreadTerminate(NULL);
				break;	
                
			case CLIENT_STATE_ERROR:
				LOG_TCP_SERVER("TCP_S_ClientTask: Error state\n");
				netbuf_delete(buf);
				osDelay(1000);
				state = CLIENT_STATE_DISCONNECT;
				break;	

            default:
                state = CLIENT_STATE_ERROR;
                break;
		}

		osDelay(10);
	}
}
