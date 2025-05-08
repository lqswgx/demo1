/*
 * @Author: LQS
 * @Date: 2025-05-07 20:45:35
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-08 01:55:01
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\ASCII\ascii_slave.c
 * @Description: 
 */
#include "ascii_slave.h"
#include  "mb_init.h"

#include  "PCF8574.h"
#include  "us_freertos.h"

#define ascii_slave_id  0x7EU

#define ASCII_SlaveRxPinState 0U
#define ASCII_SlaveTxPinState 1U

uint8_t ascii_slave_rx_buff[ASCII_RX_BUFF_LEN_MAX];
uint8_t ascii_slave_tx_buff[ASCII_TX_BUFF_LEN_MAX];

ASCII_SlaveHandleTypeDef ascii_slave;
ASCII_SlaveStateTypeDef ascii_slave_state = ASCII_SLAVE_STATE_READY;

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

void ascii_slave_init(void)
{	
	memset(&ascii_slave, 0, sizeof(ASCII_SlaveHandleTypeDef));
	memset(ascii_slave_rx_buff, 0, ASCII_RX_BUFF_LEN_MAX);
	memset(ascii_slave_tx_buff, 0, ASCII_TX_BUFF_LEN_MAX);
	RS485_Dir_Set(ASCII_SlaveRxPinState);
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, ascii_slave_rx_buff, ASCII_RX_BUFF_LEN_MAX);
}

void ascii_slave_poll(void)
{	
	switch (ascii_slave_state)
	{
		case ASCII_SLAVE_STATE_READY:
			ascii_slave_init();
			ascii_slave_state = ASCII_SLAVE_STATE_RX;
			break;
		
		case ASCII_SLAVE_STATE_RX:
			if (ascii_slave.rx_reflag)
			{
				ascii_slave_state = ASCII_SLAVE_STATE_HANDLE;
			}
			break;
			
		case ASCII_SLAVE_STATE_HANDLE:
			ascii_slave.tx_completed = 0;
			if (mb_rx_check_handle(ascii_slave_rx_buff, ascii_slave.rx_count, ascii_slave_tx_buff, &ascii_slave.tx_count))
			{ 
				RS485_Dir_Set(ASCII_SlaveTxPinState);
				HAL_UART_Transmit_DMA(&huart2, ascii_slave_tx_buff, ascii_slave.tx_count);
				while (HAL_UART_GetState(&huart2) != HAL_UART_STATE_READY)
				{
					osDelay(1);
				}
				ascii_slave.tx_completed = 1;
				ascii_slave_state = ASCII_SLAVE_STATE_TX;				
			}
			else
			{
				ascii_slave_state = ASCII_SLAVE_STATE_READY;
			}
			break;
			
		case ASCII_SLAVE_STATE_TX:
			if (ascii_slave.tx_completed)
			{
				ascii_slave_state = ASCII_SLAVE_STATE_READY;
			}
			break;
		
		case ASCII_SLAVE_STATE_TIMEOUT:
			break;
		
		default: 
			ascii_slave_state = ASCII_SLAVE_STATE_READY; 
			break;
	}
}

void SlaveAsciiRxIRQHandler(void)
{	
	uint16_t RXCount = 0;
	
	if (ascii_slave.rx_reflag) return;

	HAL_UART_DMAStop(&huart2);
    RXCount = ASCII_RX_BUFF_LEN_MAX - __HAL_DMA_GET_COUNTER(&hdma_usart2_rx);

	if (ascii_slave_rx_buff[0] == ':' && ascii_slave_rx_buff[RXCount-2] == '\r' && ascii_slave_rx_buff[RXCount-1] == '\n')
	{
		/* Verify frame end (CR LF) and slave ID */
		uint8_t slave_id = ascii_to_byte(ascii_slave_rx_buff+1);
		if (slave_id == ascii_slave_id) /* ID=01 */
		{
            /* Check function code (ASCII to HEX) */
			uint8_t func_code = ascii_to_byte(ascii_slave_rx_buff+3);
            if (func_code == 0x01 || func_code == 0x02 || func_code == 0x03 ||
                func_code == 0x04 || func_code == 0x05 || func_code == 0x06)
            {
                if(RXCount == 17)
                {
                    ascii_slave.rx_count = RXCount;
                    ascii_slave.rx_reflag = 1;
                }
            }
            else if (func_code == 0x0F || func_code == 0x10)
            {
                /* Multi-register write, length depends on byte count */
				uint8_t byte_count = ascii_to_byte(ascii_slave_rx_buff+13);
                if (RXCount == (15 + byte_count * 2 + 4)) /* : + ID + FC + ADDR + N + BYTE_COUNT + DATA + LRC + CRLF */
                {
                    ascii_slave.rx_count = RXCount;
                    ascii_slave.rx_reflag = 1;
                }
            }
		}
	}
	
	if (!ascii_slave.rx_reflag)
	{
		HAL_UART_Receive_DMA(&huart2, ascii_slave_rx_buff, ASCII_RX_BUFF_LEN_MAX);
	}
}


