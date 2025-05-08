/*
 * @Author: LQS
 * @Date: 2025-04-28 15:30:32
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-30 13:30:46
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\ATConfig\AT_Cfg.c
 * @Description: 
 */
#include "AT_Cfg.h"

// 全局 AT 指令驱动实例
AT_Cmd_t at_cmd_instance;

// 初始化 AT 模块引脚
HAL_StatusTypeDef AT_Cmd_PinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    AT_EN_GPIO_CLK_EN();
    AT_RST_GPIO_CLK_EN();
    AT_STATE_GPIO_CLK_EN();

    GPIO_InitStruct.Pin = AT_EN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(AT_EN_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = AT_RST_PIN;
    HAL_GPIO_Init(AT_RST_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = AT_STATE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(AT_STATE_GPIO_PORT, &GPIO_InitStruct);

    AT_EN_SET();
    AT_RST_SET();

    LOG_ATCMD("AT_Cmd_PinInit: OK\n");
    return HAL_OK;
}

// 初始化 AT 指令驱动
HAL_StatusTypeDef AT_Cmd_Init(AT_Cmd_t *at_cmd, UART_HandleTypeDef *huart)
{
    if (at_cmd == NULL || huart == NULL || huart->Instance == NULL)
    {
        LOG_ATCMD("AT_Cmd_Init: param error\n");
        return HAL_ERROR;
    }

    // 清空结构体
    memset(at_cmd, 0, sizeof(AT_Cmd_t));
    at_cmd->respType = AT_RESP_NONE;
	
    // 保存 UART 句柄
    at_cmd->huart = huart;

    // 使能 UART 接收中断
    AT_UART_ENABLE_RX_IT();

    // 启动单字节接收
    if (AT_UART_RECEIVE_IT(at_cmd->rxBuffer, 1) != HAL_OK)
    {
        LOG_ATCMD("AT_Cmd_Init: AT_UART_RECEIVE_IT failed\n");
        return HAL_ERROR;
	}

    LOG_ATCMD("AT_Cmd_Init: OK\n");
    return HAL_OK;
}

// 硬件复位 AT 模块
HAL_StatusTypeDef AT_Cmd_HardReset(AT_Cmd_t *at_cmd)
{
    if (at_cmd == NULL)
    {
        LOG_ATCMD("AT_Cmd_HardReset: param error\n");
        return HAL_ERROR;
    }

    // 拉低复位引脚
    AT_RST_RESET();
    vTaskDelay(pdMS_TO_TICKS(AT_RESET_DELAY_MS));
    AT_RST_SET();

    // 等待模组稳定
    vTaskDelay(pdMS_TO_TICKS(100));

    // 清空结构体
    memset(at_cmd, 0, sizeof(AT_Cmd_t));
    at_cmd->respType = AT_RESP_NONE;
    at_cmd->huart = &AT_UART_HANDLE;

    // 重新启动接收
    AT_UART_RECEIVE_IT(at_cmd->rxBuffer, 1);

    LOG_ATCMD("AT_Cmd_HardReset: OK\n");
    return HAL_OK;
}

// 发送 AT 指令并等待响应
AT_Status_t AT_Cmd_Send(AT_Cmd_t *at_cmd, const char *command, uint8_t add_crlf)
{
    if (at_cmd == NULL || command == NULL || at_cmd->huart == NULL)
    {
        LOG_ATCMD("AT_Cmd_Send: param error\n");
        return AT_STATUS_ERROR;
    }

    // 清空收发缓存
    at_cmd->rxComplete = 0;
    at_cmd->txLength = 0;
    memset(at_cmd->txBuffer, 0, AT_TX_BUFFER_SIZE);
    at_cmd->respType = AT_RESP_NONE;
    at_cmd_instance.rxLength = 0;
    memset(at_cmd_instance.rxBuffer, 0, AT_RX_BUFFER_SIZE);

    // 重新启动接收
    AT_UART_RECEIVE_IT(at_cmd_instance.rxBuffer, 1);

    // 准备发送数据
    if (add_crlf)
    {
        at_cmd->txLength = snprintf((char *)at_cmd->txBuffer, AT_TX_BUFFER_SIZE, "%s\r\n", command);
    }
    else
    {
        at_cmd->txLength = snprintf((char *)at_cmd->txBuffer, AT_TX_BUFFER_SIZE, "%s", command);
    }

    // 发送 AT 指令
    if (AT_UART_TRANSMIT(at_cmd->txBuffer, at_cmd->txLength, 1000) != HAL_OK)
    {
        LOG_ATCMD("AT_Cmd_Send: AT_UART_TRANSMIT failed\n");
        return AT_STATUS_ERROR;
    }
    else
    {
        LOG_ATCMD("AT_Cmd_Send: command: %s\n", command);
    }

    // 等待接收
    LOG_ATCMD("AT_Cmd_Send: wait response\n");
    TickType_t  start_tick = xTaskGetTickCount();
    while(at_cmd->rxComplete == 0 && xTaskGetTickCount() - start_tick < AT_RESPONSE_TIMEOUT)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    // 处理响应
    if (at_cmd->rxComplete == 0)
    {
        LOG_ATCMD("AT_Cmd_Send: response timeout\n");
        return AT_STATUS_TIMEOUT;
    }
    else
    {
        LOG_ATCMD("AT_Cmd_Send: response type: %d\n",at_cmd->respType);
        LOG_ATCMD("AT_Cmd_Send: response:\n");
        LOG_ATCMD("%.*s\n", at_cmd->response.length, at_cmd->response.buffer);
        at_cmd->response.length = 0;
        memset(at_cmd->response.buffer, 0, at_cmd->response.length);
        return AT_STATUS_OK;
    }
}

// 串口接收完成回调
void AT_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static TickType_t start_tick = 0;

    if (huart == at_cmd_instance.huart)
    {
        // 记录接收开始时间
        if (at_cmd_instance.rxLength == 0)
        {
            start_tick = xTaskGetTickCountFromISR();
        }

        // 记录接收字节数
        at_cmd_instance.rxLength++;

        // 检查缓冲区是否溢出
        if (at_cmd_instance.rxLength >= AT_RX_BUFFER_SIZE - 1)
        {
            memcpy(at_cmd_instance.response.buffer, at_cmd_instance.rxBuffer, at_cmd_instance.rxLength);
            at_cmd_instance.response.length = at_cmd_instance.rxLength;
            at_cmd_instance.rxComplete = 1;
            at_cmd_instance.respType = AT_RESP_BUFFER_OVERFLOW;
            return;
        }

        // 检查帧尾标志
        static const char *end_strs[AT_FRAME_END_COUNT] = AT_FRAME_END_STRS;
        static const uint16_t end_strs_len[AT_FRAME_END_COUNT] = AT_FRAME_END_STRS_LEN;

        for (uint8_t i = 0; i < AT_FRAME_END_COUNT; i++)
        {
            if (at_cmd_instance.rxLength >= end_strs_len[i] &&
                strncmp((char *)(at_cmd_instance.rxBuffer + at_cmd_instance.rxLength - end_strs_len[i]),
                        end_strs[i], end_strs_len[i]) == 0)
            {
                at_cmd_instance.respType = (i == 0) ? AT_RESP_OK : ((i == 1) ? AT_RESP_ERROR : AT_RESP_UNKNOWN);

                memcpy(at_cmd_instance.response.buffer, at_cmd_instance.rxBuffer, at_cmd_instance.rxLength);
                at_cmd_instance.response.length = at_cmd_instance.rxLength;
                at_cmd_instance.rxComplete = 1;
                return;
            }
        }

        // 检查超时
        if (at_cmd_instance.rxLength > 0 && xTaskGetTickCountFromISR() - start_tick >= AT_FRAME_TIMEOUT)
        {
            memcpy(at_cmd_instance.response.buffer, at_cmd_instance.rxBuffer, at_cmd_instance.rxLength);
            at_cmd_instance.response.length = at_cmd_instance.rxLength;
            at_cmd_instance.rxComplete = 1;
            at_cmd_instance.respType = AT_RESP_TIMEOUT;
            return;    
        }

        // 继续接收下一个字节
        AT_UART_RECEIVE_IT(at_cmd_instance.rxBuffer + at_cmd_instance.rxLength, 1);
    }
}

