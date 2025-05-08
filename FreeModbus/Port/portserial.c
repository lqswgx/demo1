/*
 * @Author: LQS
 * @Date: 2025-05-04 17:46:51
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-06 21:38:36
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\FreeModbus\Port\portserial.c
 * @Description: 
 */
/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#include "usart.h"

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

#define TX_BUFFER_SIZE 256
#define RX_BUFFER_SIZE 256
static uint8_t rxBuffer[RX_BUFFER_SIZE];
static QueueHandle_t xRxQueue;
static volatile uint16_t usRxCount = 0;

extern void RS485_Dir_Set(uint8_t dir);
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
    if (xRxEnable)
    {
        /* RS485 接收模式 */
        RS485_Dir_Set(0); /* DE 置低 */
        /* 启动 DMA 接收 */
        HAL_UART_Receive_DMA(&huart2, rxBuffer, RX_BUFFER_SIZE);
    } 
    else
    {
        /* 停止 DMA 接收 */
        HAL_UART_DMAStop(&huart2);
    }

    if (xTxEnable)
    {
        /* RS485 发送模式 */
        RS485_Dir_Set(1); /* DE 置高 */
    }
    else
    {
        /* RS485 接收模式 */
        RS485_Dir_Set(0); /* DE 置低 */
    }    
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    huart2.Instance = USART2;
    huart2.Init.BaudRate = ulBaudRate;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart2);

    __HAL_UART_CLEAR_IDLEFLAG(&huart2);
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE); /* 启用空闲中断 */
	vMBPortSerialEnable(TRUE,FALSE);
    
    //初始化队列
    xRxQueue = xQueueCreate(RX_BUFFER_SIZE, sizeof(uint8_t));
    return (xRxQueue != NULL);   

	// return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */

    HAL_UART_Transmit(&huart2, (uint8_t*)&ucByte, 1, 1000); 
    // static uint8_t txBuffer[TX_BUFFER_SIZE];
    // static uint16_t txIndex = 0;

    // txBuffer[txIndex++] = (uint8_t)ucByte;
    // if (pxMBFrameCBTransmitterEmpty())
    // {
    //     /* 帧发送完成，启动 DMA 发送 */
    //     HAL_UART_Transmit_DMA(&huart2, txBuffer, txIndex);
    //     txIndex = 0; /* 重置缓冲区 */
    // }
  
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
    return xQueueReceiveFromISR(xRxQueue, pucByte, 0) == pdTRUE;
    // return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

/* 空闲中断和 DMA 接收回调 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        /* 空闲中断触发，处理接收到的帧 */
        usRxCount = RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
        for (uint16_t i = 0; i < usRxCount; i++)
        {
            xQueueSendFromISR(xRxQueue, &rxBuffer[i], &xHigherPriorityTaskWoken);
            pxMBFrameCBByteReceived(); /* 通知 FreeModbus */
        }
        usRxCount = 0;
        /* 重新启动 DMA 接收 */
        HAL_UART_Receive_DMA(&huart2, rxBuffer, RX_BUFFER_SIZE);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

/* DMA 发送完成回调 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    // if (huart->Instance == USART2)
    // {
    //     /* RS485 切换回接收模式 */
    //     RS485_Dir_Set(0);
    //     pxMBFrameCBTransmitterEmpty(); /* 通知 FreeModbus */
    // }
}
