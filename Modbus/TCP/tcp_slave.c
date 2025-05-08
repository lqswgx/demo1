/*
 * @Author: LQS
 * @Date: 2025-05-07 18:22:02
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 23:25:58
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\TCP\tcp_slave.c
 * @Description: 
 */
#include "mb_init.h"
#include "tcp_slave.h"

#define tcp_slave_id 0x01U

uint8_t tcp_slave_rx_buff[TCP_RX_BUFF_LEN_MAX];
uint8_t tcp_slave_tx_buff[TCP_TX_BUFF_LEN_MAX];

TCP_SlaveHandleTypeDef tcp_slave;
TCP_SlaveStateTypeDef tcp_slave_state = TCP_SLAVE_STATE_READY;

SemaphoreHandle_t tcp_tx_sem = NULL;

void tcp_slave_init(void)
{
    memset(&tcp_slave, 0, sizeof(TCP_SlaveHandleTypeDef));
    memset(tcp_slave_rx_buff, 0, TCP_RX_BUFF_LEN_MAX);
    memset(tcp_slave_tx_buff, 0, TCP_TX_BUFF_LEN_MAX);

    if (tcp_tx_sem == NULL)
    {
        tcp_tx_sem = xSemaphoreCreateBinary();
        if (tcp_tx_sem == NULL)
        {
            tcp_slave_state = TCP_SLAVE_STATE_TIMEOUT;
            return;
        }
    }
}

void tcp_slave_poll(void)
{
    switch (tcp_slave_state)
    {
        case TCP_SLAVE_STATE_READY:
            tcp_slave_init();
            tcp_slave_state = TCP_SLAVE_STATE_RX;
            break;

        case TCP_SLAVE_STATE_RX:
            if (tcp_slave.rx_reflag)
            {
                tcp_slave_state = TCP_SLAVE_STATE_HANDLE;
            }
            break;

        case TCP_SLAVE_STATE_HANDLE:
            tcp_slave.tx_completed = 0;
            if (mb_rx_check_handle(tcp_slave_rx_buff, tcp_slave.rx_count, tcp_slave_tx_buff, &tcp_slave.tx_count))
            {
                xSemaphoreGive(tcp_tx_sem);//发送数据
                tcp_slave_state = TCP_SLAVE_STATE_TX;
            }
            else
            {
                tcp_slave_state = TCP_SLAVE_STATE_READY;
            }
            break;

        case TCP_SLAVE_STATE_TX:
            if (tcp_slave.tx_completed)
            {
                tcp_slave_state = TCP_SLAVE_STATE_READY;
            }
            break;

        case TCP_SLAVE_STATE_TIMEOUT:
            tcp_slave_state = TCP_SLAVE_STATE_READY;
            break;

        default:
            tcp_slave_state = TCP_SLAVE_STATE_READY;
            break;
    }
}

void SlaveTcpRxHandler(void)
{
    if (tcp_slave.rx_reflag)return;

    // 解析 MBAP 头部
    tcp_slave.transaction_id = (tcp_slave_rx_buff[0] << 8) | tcp_slave_rx_buff[1];
    uint16_t protocol_id = (tcp_slave_rx_buff[2] << 8) | tcp_slave_rx_buff[3];
    uint16_t length = (tcp_slave_rx_buff[4] << 8) | tcp_slave_rx_buff[5];
    tcp_slave.unit_id = tcp_slave_rx_buff[6];

    // 验证协议 ID 和单元 ID
    if (protocol_id != 0x0000 || tcp_slave.unit_id != tcp_slave_id)
        return;

    // 检查长度和功能码
    if (length != tcp_slave.rx_count - 6)
        return;

    uint8_t func_code = tcp_slave_rx_buff[7];
    if (func_code == 0x01 || func_code == 0x02 || func_code == 0x03 ||
        func_code == 0x04 || func_code == 0x05 || func_code == 0x06)
    {
        if (tcp_slave.rx_count == 12) // MBAP (7) + PDU (5)
        {
            tcp_slave.rx_reflag = 1;
        }
    }
    else if (func_code == 0x0F || func_code == 0x10)
    {
        uint8_t byte_count = tcp_slave_rx_buff[12];
        if (tcp_slave.rx_count == 13 + byte_count) // MBAP (7) + PDU (6 + byte_count)
        {
            tcp_slave.rx_reflag = 1;
        }
    }
}


