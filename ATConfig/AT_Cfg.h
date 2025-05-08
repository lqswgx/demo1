/*
 * @Author: LQS
 * @Date: 2025-04-28 15:30:40
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-04 17:32:50
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\ATConfig\AT_Cfg.h
 * @Description: 
 */
#ifndef __AT__CFG_H
#define __AT__CFG_H
#include "main.h"
#include "us_freertos.h"
#include "usart.h"

// 串口操作宏定义
#define AT_UART_HANDLE          huart3

#define AT_UART_TRANSMIT(data, size, timeout) \
    HAL_UART_Transmit(&AT_UART_HANDLE, (uint8_t *)(data), (size), (timeout))

#define AT_UART_RECEIVE_IT(data, size) \
    HAL_UART_Receive_IT(&AT_UART_HANDLE, (data), (size))  

#define AT_UART_ENABLE_RX_IT() \
    __HAL_UART_ENABLE_IT(&AT_UART_HANDLE, UART_IT_RXNE)

#define AT_UART_DISABLE_RX_IT() \
__HAL_UART_DISABLE_IT(&AT_UART_HANDLE, UART_IT_RXNE)

// GPIO 引脚宏定义
#define AT_EN_PIN               GPIO_PIN_0
#define AT_EN_GPIO_PORT         GPIOA
#define AT_EN_GPIO_CLK_EN       __HAL_RCC_GPIOA_CLK_ENABLE
#define AT_RST_PIN              GPIO_PIN_1
#define AT_RST_GPIO_PORT        GPIOA
#define AT_RST_GPIO_CLK_EN      __HAL_RCC_GPIOA_CLK_ENABLE
#define AT_STATE_PIN            GPIO_PIN_2
#define AT_STATE_GPIO_PORT      GPIOA
#define AT_STATE_GPIO_CLK_EN    __HAL_RCC_GPIOA_CLK_ENABLE

// 引脚操作宏
#define AT_EN_SET()             HAL_GPIO_WritePin(AT_EN_GPIO_PORT, AT_EN_PIN, GPIO_PIN_SET)
#define AT_EN_RESET()           HAL_GPIO_WritePin(AT_EN_GPIO_PORT, AT_EN_PIN, GPIO_PIN_RESET)
#define AT_RST_SET()            HAL_GPIO_WritePin(AT_RST_GPIO_PORT, AT_RST_PIN, GPIO_PIN_SET)
#define AT_RST_RESET()          HAL_GPIO_WritePin(AT_RST_GPIO_PORT, AT_RST_PIN, GPIO_PIN_RESET)
#define AT_STATE_READ()         HAL_GPIO_ReadPin(AT_STATE_GPIO_PORT, AT_STATE_PIN)

// 配置参数
#define AT_TX_BUFFER_SIZE       16
#define AT_RX_BUFFER_SIZE       512
#define AT_RESPONSE_TIMEOUT     pdMS_TO_TICKS(2000)
#define AT_FRAME_TIMEOUT        pdMS_TO_TICKS(500)
#define AT_RESET_DELAY_MS       100

#define AT_FRAME_END_STRS       {"\r\nOK\r\n", "\r\nERROR\r\n", "Unknown cmd:"}
#define AT_FRAME_END_STRS_LEN   {6, 9, 12}
#define AT_FRAME_END_COUNT      3

#define AT_DEFAULT_CRLF         1

// AT 指令执行状态
typedef enum
{
    AT_STATUS_OK = 0,
    AT_STATUS_ERROR,
    AT_STATUS_TIMEOUT,
    AT_STATUS_BUFFER_OVERFLOW
} AT_Status_t;

// AT 模块响应状态
typedef enum
{
    AT_RESP_OK = 0,
    AT_RESP_ERROR,
    AT_RESP_UNKNOWN,
    AT_RESP_CUSTOM,
    AT_RESP_BUFFER_OVERFLOW,
    AT_RESP_TIMEOUT,
    AT_RESP_NONE
} AT_Response_t;

// 帧数据结构
typedef struct
{
    uint8_t buffer[AT_RX_BUFFER_SIZE];
    uint16_t length;
} AT_Frame_t;

// AT 指令驱动句柄
typedef struct
{
    UART_HandleTypeDef *huart;
    uint8_t rxBuffer[AT_RX_BUFFER_SIZE];
    uint16_t rxLength;
    uint8_t txBuffer[AT_TX_BUFFER_SIZE];
    uint16_t txLength;
    bool rxComplete;
    AT_Response_t respType;
    AT_Frame_t response;
} AT_Cmd_t;

// 全局 AT 指令驱动实例
extern AT_Cmd_t at_cmd_instance;

// 函数声明
HAL_StatusTypeDef AT_Cmd_PinInit(void);
HAL_StatusTypeDef AT_Cmd_Init(AT_Cmd_t *at_cmd, UART_HandleTypeDef *huart);
HAL_StatusTypeDef AT_Cmd_HardReset(AT_Cmd_t *at_cmd);
AT_Status_t AT_Cmd_Send(AT_Cmd_t *at_cmd, const char *command, uint8_t add_crlf);
void AT_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif





 

