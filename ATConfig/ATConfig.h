/*
 * @Author: LQS
 * @Date: 2025-04-27 14:10:05
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-27 17:31:06
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\ATConfig\ATConfig.h
 * @Description: 
 */
#ifndef __AT_CONFIG_H_
#define __AT_CONFIG_H_
#include "main.h"
#include "usart.h"
#include "us_freertos.h"

// 串口操作宏定义，适配已初始化的串口
#define AT_UART_HANDLE          huart3               // UART 句柄
#define AT_UART_RX_DMA_HANDLE   hdma_usart3_rx       // UART 接收 DMA 句柄
#define AT_UART_TX_DMA_HANDLE   hdma_usart3_tx       // UART 发送 DMA 句柄

#define AT_UART_TRANSMIT(data, size, timeout) \
    HAL_UART_Transmit_DMA(&AT_UART_HANDLE, (uint8_t *)(data), (size)) // DMA 发送

#define AT_UART_RECEIVE(data, size) \
    HAL_UART_Receive_DMA(&AT_UART_HANDLE, (uint8_t *)(data), (size)) // DMA 接收

#define AT_UART_GET_RX_COUNT() \
    (AT_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&AT_UART_RX_DMA_HANDLE)) // 获取 DMA 接收计数

#define AT_UART_ENABLE_IDLE_IT() \
    __HAL_UART_ENABLE_IT(&AT_UART_HANDLE, UART_IT_IDLE) // 使能空闲中断

#define AT_UART_CLEAR_IDLE_FLAG() \
    __HAL_UART_CLEAR_IDLEFLAG(&AT_UART_HANDLE); // 清除空闲中断标志


// GPIO 引脚宏定义
#define AT_EN_PIN               GPIO_PIN_0           // 使能引脚 (示例：PA0)
#define AT_EN_GPIO_PORT         GPIOA                // 使能引脚端口
#define AT_EN_GPIO_CLK_EN       __HAL_RCC_GPIOA_CLK_ENABLE // 使能引脚时钟
#define AT_RST_PIN              GPIO_PIN_1           // 复位引脚 (示例：PA1)
#define AT_RST_GPIO_PORT        GPIOA                // 复位引脚端口
#define AT_RST_GPIO_CLK_EN      __HAL_RCC_GPIOA_CLK_ENABLE // 复位引脚时钟
#define AT_STATE_PIN            GPIO_PIN_2           // 状态引脚 (示例：PA2)
#define AT_STATE_GPIO_PORT      GPIOA                // 状态引脚端口
#define AT_STATE_GPIO_CLK_EN    __HAL_RCC_GPIOA_CLK_ENABLE // 状态引脚时钟

// 引脚操作宏
#define AT_EN_SET()             HAL_GPIO_WritePin(AT_EN_GPIO_PORT, AT_EN_PIN, GPIO_PIN_SET) // 使能高电平
#define AT_EN_RESET()           HAL_GPIO_WritePin(AT_EN_GPIO_PORT, AT_EN_PIN, GPIO_PIN_RESET) // 使能低电平
#define AT_RST_SET()            HAL_GPIO_WritePin(AT_RST_GPIO_PORT, AT_RST_PIN, GPIO_PIN_SET) // 复位高电平
#define AT_RST_RESET()          HAL_GPIO_WritePin(AT_RST_GPIO_PORT, AT_RST_PIN, GPIO_PIN_RESET) // 复位低电平
#define AT_STATE_READ()         HAL_GPIO_ReadPin(AT_STATE_GPIO_PORT, AT_STATE_PIN) // 读取状态引脚

// 缓冲区和超时配置
#define AT_TX_BUFFER_SIZE       32                   // 发送缓冲区大小
#define AT_RX_BUFFER_SIZE       256                  // 接收缓冲区大小
#define AT_RESPONSE_TIMEOUT     pdMS_TO_TICKS(2000)  // 响应超时（FreeRTOS ticks）
#define AT_RESET_DELAY_MS       100                  // 复位脉冲时间（ms）
#define AT_RESET_RESPONSE_TIMEOUT pdMS_TO_TICKS(5000) // 软复位响应超时（较长以适应重启）
#define AT_RESET_COMMAND        "AT+RST"             // 默认软复位指令


// 默认是否追加回车换行符
#define AT_DEFAULT_CRLF         1                    // 1: 追加 \r\n, 0: 不追加

// AT 指令执行状态
typedef enum
{
    AT_STATUS_OK = 0,    // 指令执行成功
    AT_STATUS_ERROR,     // 指令执行失败
    AT_STATUS_TIMEOUT,   // 响应超时
    AT_STATUS_BUFFER_OVERFLOW // 接收缓冲区溢出
} AT_Status_t;

// AT 模块响应状态
typedef enum
{
    AT_RESP_OK = 0,      // 响应为 OK
    AT_RESP_ERROR,       // 响应为 ERROR
    AT_RESP_CUSTOM,      // 自定义响应
    AT_RESP_NONE         // 无有效响应
} AT_Response_t;

// AT 指令驱动句柄
typedef struct
{
    UART_HandleTypeDef *huart;          // UART 句柄
    QueueHandle_t rxQueue;              // 接收队列
    char sendbuff[AT_TX_BUFFER_SIZE];   // 发送缓冲区
    uint16_t send_len;                  // 发送数据长度
    char response[AT_RX_BUFFER_SIZE];   // 接收到的回应数据
    uint16_t response_len;              // 回应数据长度 
    bool rxComplete;                    // 接收完成标志
    AT_Response_t resp_type;            // 回应类型   
} AT_Cmd_t;
extern AT_Cmd_t at_cmd_instance;

// 函数声明
HAL_StatusTypeDef AT_Cmd_PinInit(void);
HAL_StatusTypeDef AT_Cmd_Init(AT_Cmd_t *at_cmd, UART_HandleTypeDef *huart);
HAL_StatusTypeDef AT_Cmd_HardReset(AT_Cmd_t *at_cmd);
HAL_StatusTypeDef AT_Cmd_SoftReset(AT_Cmd_t *at_cmd);
AT_Status_t AT_Cmd_Send(AT_Cmd_t *at_cmd, const char *command, uint8_t add_crlf);
void AT_Cmd_RxIdleCallback(UART_HandleTypeDef *huart);

#endif





 

