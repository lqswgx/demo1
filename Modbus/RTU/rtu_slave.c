/*
 * @Author: LQS
 * @Date: 2025-05-06 21:47:56
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 23:49:18
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\RTU\rtu_slave.c
 * @Description: 
 */
#include  "mb_init.h"
#include  "rtu_slave.h"

#include  "PCF8574.h"
#include  "us_freertos.h"

#define  rtu_slave_id            	0x01U

#define  RTU_SlaveRxPinState     	0U
#define  RTU_SlaveTxPinState     	1U

uint8_t  rtu_slave_rx_buff[RTU_RX_BUFF_LEN_MAX];
uint8_t  rtu_slave_tx_buff[RTU_TX_BUFF_LEN_MAX];

RTU_SlaveHandleTypeDef rtu_slave;
RTU_SlaveStateTypeDef  rtu_slave_state=RTU_SLAVE_STATE_READY;

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

void rtu_slave_init(void)
{	
	memset(&rtu_slave,0,sizeof(RTU_SlaveHandleTypeDef));
	memset(rtu_slave_rx_buff,0,RTU_RX_BUFF_LEN_MAX);
	memset(rtu_slave_tx_buff,0,RTU_TX_BUFF_LEN_MAX);
	RS485_Dir_Set(RTU_SlaveRxPinState);
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2,rtu_slave_rx_buff,RTU_RX_BUFF_LEN_MAX);
//	DMA1_Channel5->CNDTR = Slave_rx_dmabuff_size; 
//	DMA_Cmd(DMA1_Channel5, ENABLE);
//	Slave_RTS(RTU_SlaveRxPinState);
}

void rtu_slave_poll(void)
{	
	switch(rtu_slave_state)
	{
		case   RTU_SLAVE_STATE_READY:
			rtu_slave_init();
			rtu_slave_state=RTU_SLAVE_STATE_RX;
		break;
		
		case   RTU_SLAVE_STATE_RX:
			if(rtu_slave.rx_reflag)
			{
				rtu_slave_state=RTU_SLAVE_STATE_HANDLE;
			}
		break;
			
		case   RTU_SLAVE_STATE_HANDLE:
			rtu_slave.tx_completed=0;
			if(mb_rx_check_handle(rtu_slave_rx_buff, rtu_slave.rx_count, rtu_slave_tx_buff, &rtu_slave.tx_count))
			{ 
				RS485_Dir_Set(RTU_SlaveTxPinState);
				HAL_UART_Transmit_DMA(&huart2,rtu_slave_tx_buff,rtu_slave.tx_count);
				while(HAL_UART_GetState(&huart2)!= HAL_UART_STATE_READY)
				{
					osDelay(1);
				}
				rtu_slave.tx_completed=1;
				// SlaveMBPortSerial_DMASendData(rtu_slave.tx_count);
//				USART_SendData(USART1,rtu_slave_tx_buff[0]);
//				USART_ITConfig(USART1, USART_IT_TC, ENABLE);
				rtu_slave_state=RTU_SLAVE_STATE_TX;				
			}
			else
			{
				rtu_slave_state=RTU_SLAVE_STATE_READY;
			} 
		break;
			
		case   RTU_SLAVE_STATE_TX:
			if(rtu_slave.tx_completed)
			{
				rtu_slave_state=RTU_SLAVE_STATE_READY;
			}
		break;
		
		case   RTU_SLAVE_STATE_TIMEOUT:

		break;
		
		default: 
			rtu_slave_state=RTU_SLAVE_STATE_READY; 
		break;
	}
}

void SlaveRtuRxIRQHandler(void)
{	
	uint16_t RXCount=0;
	
	if(rtu_slave.rx_reflag)return;

	HAL_UART_DMAStop(&huart2);
	// DMA_Cmd(DMA1_Channel5, DISABLE);	
	if(rtu_slave_rx_buff[0]==rtu_slave_id)
	{
		RXCount=RTU_RX_BUFF_LEN_MAX-__HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		// RXCount=Slave_rx_dmabuff_size-DMA_GetCurrDataCounter(DMA1_Channel5);
		if(rtu_slave_rx_buff[1]==0x01 || rtu_slave_rx_buff[1]==0x02 || rtu_slave_rx_buff[1]==0x03 || 
			rtu_slave_rx_buff[1]==0x04 || rtu_slave_rx_buff[1]==0x05 || rtu_slave_rx_buff[1]==0x06)
		{
			if(RXCount==8)
			{
				rtu_slave.rx_count=RXCount;
				rtu_slave.rx_reflag=1;			
			}
		}
		else if(rtu_slave_rx_buff[1]==0x0f || rtu_slave_rx_buff[1]==0x10)
		{
			if(RXCount==rtu_slave_rx_buff[6]+9)
			{
				rtu_slave.rx_count=RXCount;
				rtu_slave.rx_reflag=1;			
			}			
		}		
	}	
	
	if(!rtu_slave.rx_reflag)
	{
		HAL_UART_Receive_DMA(&huart2, rtu_slave_rx_buff, RTU_RX_BUFF_LEN_MAX);
//		DMA1_Channel5->CNDTR = Slave_rx_dmabuff_size; 
//		DMA_Cmd(DMA1_Channel5, ENABLE); 
	}
}
