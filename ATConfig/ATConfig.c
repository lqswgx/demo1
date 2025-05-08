/*
 * @Author: LQS
 * @Date: 2025-04-27 14:09:58
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-28 10:47:18
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\ATConfig\ATConfig.c
 * @Description: 
 */
#include "ATConfig.h"

// AT 指令驱动实例
AT_Cmd_t at_cmd_instance;

// 初始化 AT 模块引脚
HAL_StatusTypeDef AT_Cmd_PinInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能 GPIO 时钟
    AT_EN_GPIO_CLK_EN();
    AT_RST_GPIO_CLK_EN();
    AT_STATE_GPIO_CLK_EN();

    // 配置使能引脚 (输出)
    GPIO_InitStruct.Pin = AT_EN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(AT_EN_GPIO_PORT, &GPIO_InitStruct);

    // 配置复位引脚 (输出)
    GPIO_InitStruct.Pin = AT_RST_PIN;
    HAL_GPIO_Init(AT_RST_GPIO_PORT, &GPIO_InitStruct);

    // 配置状态引脚 (输入)
    GPIO_InitStruct.Pin = AT_STATE_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    HAL_GPIO_Init(AT_STATE_GPIO_PORT, &GPIO_InitStruct);

    // 设置默认状态
    AT_EN_SET();    // 使能模组
    AT_RST_SET();   // 保持非复位状态

    LOG_ATCMD("AT_Cmd_PinInit: OK\n");
    return HAL_OK;
}

// 初始化 AT 指令驱动
HAL_StatusTypeDef AT_Cmd_Init(AT_Cmd_t *at_cmd, UART_HandleTypeDef *huart)
{
    if (huart == NULL)
    {
        LOG_ATCMD("AT_Cmd_Init: Invalid huart\n");
        return HAL_ERROR;
    }
    
    // 清空结构体，确保所有成员初始化为 0
    memset(at_cmd, 0, sizeof(AT_Cmd_t));

    // 保存驱动实例z
    at_cmd->huart = huart;

    // 初始化驱动句柄
    at_cmd->resp_type = AT_RESP_NONE;
    at_cmd->response_len = 0;
    at_cmd->rxComplete = 0;
    memset(at_cmd->response, 0, AT_RX_BUFFER_SIZE);
    at_cmd->send_len = 0;
    memset(at_cmd->sendbuff, 0, AT_TX_BUFFER_SIZE);

    //创建接收队列
    at_cmd->rxQueue = xQueueCreate(8, sizeof(AT_Cmd_t));
    if(at_cmd->rxQueue == NULL)
    {
        LOG_ATCMD("AT_Cmd_Init: Failed to create rxQueue\n");
        return HAL_ERROR;
    }

    // 清除 UART 空闲中断标志
    AT_UART_CLEAR_IDLE_FLAG();

    // 使能 UART 空闲中断
    AT_UART_ENABLE_IDLE_IT();

    // 启动 DMA 接收
    AT_UART_RECEIVE(at_cmd->response, AT_RX_BUFFER_SIZE);

    LOG_ATCMD("AT_Cmd_Init: OK\n");
    return HAL_OK;
}

// 硬件复位 AT 模块
HAL_StatusTypeDef AT_Cmd_HardReset(AT_Cmd_t *at_cmd)
{
    // 拉低复位引脚
    AT_RST_RESET();
    vTaskDelay(pdMS_TO_TICKS(AT_RESET_DELAY_MS)); // 保持复位时间
    AT_RST_SET();

    // 等待模组稳定（根据 Ai-WB2-01S 数据手册，约 100ms）
    vTaskDelay(pdMS_TO_TICKS(100));

    // 清空接收缓冲区
    at_cmd->resp_type = AT_RESP_NONE;
    at_cmd->response_len = 0;
    at_cmd->rxComplete = 0;
    memset(at_cmd->response, 0, AT_RX_BUFFER_SIZE);
    at_cmd->send_len = 0;
    memset(at_cmd->sendbuff, 0, AT_TX_BUFFER_SIZE);    

    
    // 重新启动 DMA 接收
    AT_UART_RECEIVE(at_cmd->response, AT_RX_BUFFER_SIZE);

    LOG_ATCMD("AT_Cmd_HardReset: OK\n");
    return HAL_OK;
}

// 软件复位 AT 模块
HAL_StatusTypeDef AT_Cmd_SoftReset(AT_Cmd_t *at_cmd)
{
    // 清空接收缓冲区和标志
    at_cmd->resp_type = AT_RESP_NONE;
    at_cmd->response_len = 0;
    at_cmd->rxComplete = 0;
    memset(at_cmd->response, 0, AT_RX_BUFFER_SIZE);
    at_cmd->send_len = 0;
    memset(at_cmd->sendbuff, 0, AT_TX_BUFFER_SIZE);    

    // 发送复位指令
    AT_Status_t status = AT_Cmd_Send(at_cmd, AT_RESET_COMMAND, AT_DEFAULT_CRLF);

    // 检查响应
    if (status == AT_STATUS_OK && (at_cmd->resp_type == AT_RESP_OK || at_cmd->resp_type == AT_RESP_CUSTOM)) {
        // 等待模组重启完成
        vTaskDelay(pdMS_TO_TICKS(100));

        // 重新启动 DMA 接收
        AT_UART_RECEIVE((uint8_t *)at_cmd->response, AT_RX_BUFFER_SIZE);
        LOG_ATCMD("AT_Cmd_SoftReset: OK\n");
        return HAL_OK;
    }

    LOG_ATCMD("AT_Cmd_SoftReset: Failed\n");
    // 复位失败
    return HAL_ERROR;
}

// 发送 AT 指令并等待响应
AT_Status_t AT_Cmd_Send(AT_Cmd_t *at_cmd, const char *command, uint8_t add_crlf)
{
    if (command == NULL)
    {
        LOG_ATCMD("AT_Cmd_Send: Invalid command\n");
        return AT_STATUS_ERROR;
    }

    // 清空接收缓冲区和标志
    at_cmd->resp_type = AT_RESP_NONE;
    at_cmd->response_len = 0;
    at_cmd->rxComplete = 0;
    memset(at_cmd->response, 0, AT_RX_BUFFER_SIZE);
    at_cmd->send_len = 0;
    memset(at_cmd->sendbuff, 0, AT_TX_BUFFER_SIZE);     

    if (add_crlf) 
    {
        snprintf(at_cmd->sendbuff, AT_TX_BUFFER_SIZE, "%s\r\n", command); // 追加 \r\n
    } 
    else 
    {
        snprintf(at_cmd->sendbuff, AT_TX_BUFFER_SIZE, "%s", command); // 不追加
    }

    if (AT_UART_TRANSMIT(at_cmd->sendbuff, strlen(at_cmd->sendbuff), 1000) != HAL_OK)
    {
        LOG_ATCMD("AT_Cmd_Send: Failed to transmit\n");
        return AT_STATUS_ERROR;
    }
    else
    {
        LOG_ATCMD("AT_Cmd_Send: Transmit: %s\n", at_cmd->sendbuff);
    }

    LOG_ATCMD("AT_Cmd_Send: Waiting for transmit complete\n");

    // 等待发送完成（视需求可移除）
    while (HAL_UART_GetState(&AT_UART_HANDLE) == HAL_UART_STATE_BUSY_TX) 
    {
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    LOG_ATCMD("AT_Cmd_Send: Waiting for response\n");

	//等待响应
    if(xQueueReceive(at_cmd->rxQueue, &at_cmd_instance, portMAX_DELAY) == pdTRUE)
    {
        LOG_ATCMD("AT_Cmd_Send: response:%s\n", at_cmd->response);
    
        if (at_cmd->rxComplete)
        {
            if (strstr((char *)at_cmd->response, "OK") != NULL)
            {
                at_cmd->resp_type = AT_RESP_OK;
                return AT_STATUS_OK;
            } 
            else if (strstr((char *)at_cmd->response, "ERROR") != NULL)
            {
                at_cmd->resp_type = AT_RESP_ERROR;
                return AT_STATUS_OK;
            } 
            else
            {
                at_cmd->resp_type = AT_RESP_CUSTOM; // 自定义响应
                return AT_STATUS_OK;
            }
        } 
        else
        {
            LOG_ATCMD("AT_Cmd_Send: Response timeout\n");
            at_cmd->resp_type = AT_RESP_NONE;
            return AT_STATUS_BUFFER_OVERFLOW;
        }
    }
	
	LOG_ATCMD("AT_Cmd_Send: Response timeout\n");
	at_cmd->resp_type = AT_RESP_NONE;
	return AT_STATUS_TIMEOUT;
}

// 串口空闲中断回调（定义在驱动中）
void AT_Cmd_RxIdleCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken;

    if (huart == at_cmd_instance.huart)
    {
        if (__HAL_UART_GET_FLAG(huart, UART_FLAG_IDLE) != RESET)
        {
            AT_UART_CLEAR_IDLE_FLAG(); //清除空闲标志
    
            // 获取 DMA 接收到的数据长度
            at_cmd_instance.response_len = AT_UART_GET_RX_COUNT();
            if (at_cmd_instance.response_len > 0)
            {
                at_cmd_instance.rxComplete = 1; // 标记接收完成
            }

            xQueueSendFromISR(at_cmd_instance.rxQueue, &at_cmd_instance, &xHigherPriorityTaskWoken);

            // // 重新启动 DMA 接收
            // AT_UART_RECEIVE(at_cmd_instance.response, AT_RX_BUFFER_SIZE);
        }
    }
}
