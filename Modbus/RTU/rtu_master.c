 /*
 * @Author: LQS
 * @Date: 2025-05-06 21:47:56
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-08 18:03:04
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\RTU\rtu_master.c
 * @Description: 
 */
#include  "rtu_master.h"

#include "mb_init.h"

#include  "PCF8574.h"
#include  "us_freertos.h"

#define MasterRxPinState           0U
#define MasterTxPinState           1U

#define MasterRxTimeOutConst       1000U
#define MasterRxErrorCount         3U

MBRTUFormatTypeDef        MasterTxFormat;
RTU_MasterHandleTypeDef   rtu_master;
RTU_MasterStateTypeDef    RTU_MASTER_STATE = RTU_MASTER_STATE_READY;

uint8_t MasterRxBuffTable[MasterRXBuffLenMax];
uint8_t MasterTxBuffTable[MasterTXBuffLenMax];

MasterRxHandleFunc master_rx_handle[17]=
{
	MasterRxErrHandle,
	MasterRx01Handle,
	MasterRx02Handle,
	MasterRx03Handle,
	MasterRx04Handle,
	MasterRx05Handle,
	MasterRx06Handle,
	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
	MasterRx0FHandle,
	MasterRx10Handle
};

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;

void rtu_master_init(void)
{
	memset(MasterRxBuffTable,0,MasterRXBuffLenMax);
	memset(MasterTxBuffTable,0,MasterTXBuffLenMax);
	memset(&rtu_master,0,sizeof(RTU_MasterHandleTypeDef));
	
	RS485_Dir_Set(MasterTxPinState);
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);
	// HAL_UART_Receive_DMA(&huart2, (uint8_t*)&MasterRxBuffTable[0], MasterRXBuffLenMax);	
}

void  rtu_master_poll(void)
{	
	uint16_t RxCRC=0,RtuCRC=0;

	if(MasterTxFormat.ExecutionEventsState != ExecuteEnabled) 
	{
		return;
	}

	switch(RTU_MASTER_STATE)
	{
		case   RTU_MASTER_STATE_READY:
			rtu_master_init();
			if(MasterTxDataHandle())
			{
				RTU_MASTER_STATE=RTU_MASTER_STATE_TX;
			}	
			break;
					 
		case   RTU_MASTER_STATE_TX:
			HAL_UART_Transmit_DMA(&huart2, (uint8_t*)MasterTxBuffTable, rtu_master.tx_count);
			while(HAL_UART_GetState(&huart2)!= HAL_UART_STATE_READY)
			{
				osDelay(1);
			}
            //  MasterMBPortSerial_DMASendData(rtu_master.tx_count);
//			USART_SendData(USART2,MasterTxBuffTable[0]);
//			USART_ITConfig(USART2, USART_IT_TC, ENABLE);
			rtu_master.tx_completed=1;
			
			RS485_Dir_Set(MasterRxPinState);
			HAL_UART_Receive_DMA(&huart2, (uint8_t*)&MasterRxBuffTable[0], MasterRXBuffLenMax);
			RTU_MASTER_STATE=RTU_MASTER_STATE_HANDLE;
			break;
		
		case   RTU_MASTER_STATE_HANDLE:
			if(rtu_master.tx_completed==1) 
			{                
                // rtu_master.rx_time_out=MasterRxTimeOutConst;
				rtu_master.rx_time_out=xTaskGetTickCount();
                RTU_MASTER_STATE=RTU_MASTER_STATE_RX;
			} 
			break;
			
		case   RTU_MASTER_STATE_RX:
			if((xTaskGetTickCount()-rtu_master.rx_time_out)>MasterRxTimeOutConst)
			{
				RTU_MASTER_STATE = RTU_MASTER_STATE_TIMEOUT;
			}
			else
			{
				if(rtu_master.rx_reflag)
				{
					if(rtu_master.rx_func_code_err)
					{
						master_rx_handle[0]();
						RTU_MASTER_STATE = RTU_MASTER_STATE_TIMEOUT;
						return;
					}
					
					RtuCRC=CRC16(MasterRxBuffTable,rtu_master.rx_datacount-2);
					RxCRC=MasterRxBuffTable[rtu_master.rx_datacount-1]<<8|MasterRxBuffTable[rtu_master.rx_datacount-2];
					if(RtuCRC==RxCRC)
					{
						master_rx_handle[MasterRxBuffTable[1]]();
						RTU_MASTER_STATE = RTU_MASTER_STATE_COMPLETED;
					}
					else
					{
						RTU_MASTER_STATE = RTU_MASTER_STATE_TIMEOUT;
					}
				}
			}		
			break;
						
		case   RTU_MASTER_STATE_TIMEOUT: //timeout and error handle
			if(rtu_master.rx_errorcount<MasterRxErrorCount) 
			{
				rtu_master.rx_errorcount++;
				RTU_MASTER_STATE=RTU_MASTER_STATE_TX;	
			}
			else
			{
				RTU_MASTER_STATE =RTU_MASTER_STATE_COMPLETED;
				// RxCommunicationTimeoutFlag=SET;
			}
			break;
			
		case   RTU_MASTER_STATE_COMPLETED:
			MasterTxFormat.ExecutionEventsState = ExecuteDisabled;
			RTU_MASTER_STATE =RTU_MASTER_STATE_READY;
			break;
		
		default:
			MasterTxFormat.ExecutionEventsState = ExecuteDisabled;
			RTU_MASTER_STATE =RTU_MASTER_STATE_READY;
			break;
	}		
}

void MasterRtuRxIRQHandler(void)
{ 
	uint16_t RxDataNum=0;
	
	if(rtu_master.rx_reflag)
	{
		return;
	}

	HAL_UART_DMAStop(&huart2);	
	// DMA_Cmd(DMA1_Channel6, DISABLE);	
	if(MasterRxBuffTable[0]==MasterTxFormat.slave_addr)
	{
		RxDataNum=MasterRXBuffLenMax-__HAL_DMA_GET_COUNTER(&hdma_usart2_rx);
		// RxDataNum=Master_rx_dmabuff_size-DMA_GetCurrDataCounter(DMA1_Channel6);

		if(MasterRxBuffTable[1]==0x01 || MasterRxBuffTable[1]==0x02 || MasterRxBuffTable[1]==0x03 || MasterRxBuffTable[1]==0x04)
		{
			if(RxDataNum==MasterRxBuffTable[2]+5)
			{
				rtu_master.rx_reflag=1;
				rtu_master.rx_datacount=RxDataNum;				
			}			
		}
		else if(MasterRxBuffTable[1]==0x05 || MasterRxBuffTable[1]==0x06 || MasterRxBuffTable[1]==0x0f || MasterRxBuffTable[1]==0x10)
		{
			if(RxDataNum==8)
			{
				rtu_master.rx_reflag=1;
				rtu_master.rx_datacount=RxDataNum;
			}
		}
		else if(MasterRxBuffTable[1]==0x80+MasterTxBuffTable[1])
		{
			if(RxDataNum>=3)
			{
				rtu_master.rx_func_code_err=1;
				rtu_master.rx_reflag=1;
				rtu_master.rx_datacount=RxDataNum;				
			}			
		}
	}	
	
	if(!rtu_master.rx_reflag)
	{
		HAL_UART_Receive_DMA(&huart2, (uint8_t*)&MasterRxBuffTable[0], MasterRXBuffLenMax);
		// DMA1_Channel6->CNDTR = Master_rx_dmabuff_size; 
		// DMA_Cmd(DMA1_Channel6, ENABLE);
	}	
}

uint8_t  MasterTxDataHandle(void)
{
	bool      TxDataEvent=false;
	uint8_t   i=0,j=0;
	uint16_t  rtu_crc=0;

	memset(MasterTxBuffTable,0,sizeof(MasterTxBuffTable));

	MasterTxBuffTable[i++]=MasterTxFormat.slave_addr;
	MasterTxBuffTable[i++]=MasterTxFormat.func_code;
	MasterTxBuffTable[i++]=(MasterTxFormat.start_addr>>8)&0xff;
	MasterTxBuffTable[i++]=MasterTxFormat.start_addr&0xff;
 	
	switch(MasterTxFormat.func_code)
	{ 
		case 0x01:
			MasterTxBuffTable[i++]=(MasterTxFormat.data_num>>8)&0xff;
			MasterTxBuffTable[i++]=MasterTxFormat.data_num&0xff;
			TxDataEvent=true;
			break;

		case 0x02:
			MasterTxBuffTable[i++]=(MasterTxFormat.data_num>>8)&0xff;
			MasterTxBuffTable[i++]=MasterTxFormat.data_num&0xff;
			TxDataEvent=true;			
			break;

		case 0x03:
			MasterTxBuffTable[i++]=(MasterTxFormat.data_num>>8)&0xff;
			MasterTxBuffTable[i++]=MasterTxFormat.data_num&0xff;
			TxDataEvent=true;
			break;
		
		case 0x04:
			MasterTxBuffTable[i++]=(MasterTxFormat.data_num>>8)&0xff;
			MasterTxBuffTable[i++]=MasterTxFormat.data_num&0xff;
			TxDataEvent=true;
			break;

		case 0x05:
			MasterTxBuffTable[i++]= (MasterTxFormat.bool_val)?0xFF:0x00;
			MasterTxBuffTable[i++]=0x00;
			TxDataEvent=true;
			break;

		case 0x06:
			MasterTxBuffTable[i++]=(MasterTxFormat.reg_val[0]>>8)&0xff;
			MasterTxBuffTable[i++]=MasterTxFormat.reg_val[0]&0xff;
			TxDataEvent=true;
			break;
		
		case 0x0f:
			MasterTxBuffTable[i++]=(MasterTxFormat.data_num>>8)&0xff;
			MasterTxBuffTable[i++]=MasterTxFormat.data_num&0xff;
			MasterTxFormat.data_bytes=(MasterTxFormat.data_num+7)/8;
			MasterTxBuffTable[i++]=MasterTxFormat.data_bytes;
			if(MasterTxFormat.data_bytes<=RTU_MASTER_TX_COIL_BYTES)
			{
				for(j=0;j<MasterTxFormat.data_bytes;j++)
				{
					MasterTxBuffTable[i++]= MasterTxFormat.coil_val[j];
				}
				TxDataEvent=true;
			}
			else
			{
				LOG_MBRTU("rtu_master: MasterTxDataHandle: coil_num>=RTU_MASTER_TX_COIL_BYTES error!\r\n");
				TxDataEvent=false;
			}
			break;

		case 0x10:
			MasterTxBuffTable[i++]=(MasterTxFormat.data_num>>8)&0xff;
			MasterTxBuffTable[i++]=MasterTxFormat.data_num&0xff;
			MasterTxFormat.data_bytes=MasterTxFormat.data_num*2;
			MasterTxBuffTable[i++]=MasterTxFormat.data_bytes;
			if(MasterTxFormat.data_bytes<=RTU_MASTER_TX_REG_BYTES)
			{
				for(j=0;j<MasterTxFormat.data_num;j++)
				{
					MasterTxBuffTable[i++]=(MasterTxFormat.reg_val[j]>>8)&0xff;
					MasterTxBuffTable[i++]=MasterTxFormat.reg_val[j]&0xff;					
				}
				TxDataEvent=true;
			}
			else
			{
				LOG_MBRTU("rtu_master: MasterTxDataHandle: reg_num>=RTU_MASTER_TX_REG_BYTES error!\r\n");
				TxDataEvent=false;
			}
			break;	
			
		default:
			LOG_MBRTU("rtu_master: MasterTxDataHandle: func_code error!\r\n");
			TxDataEvent=false;
			break;
	}

	if(TxDataEvent)
	{
		rtu_crc=CRC16(MasterTxBuffTable,i);
		MasterTxBuffTable[i++]=rtu_crc&0xff;
		MasterTxBuffTable[i++]=(rtu_crc>>8)&0xff;
		rtu_master.tx_count=i;

		if(rtu_master.tx_count>MasterTXBuffLenMax)
		{
			LOG_MBRTU("rtu_master: MasterTxDataHandle: tx_count > MasterTXBuffLenMax error!\r\n");
			TxDataEvent=false;
		}
	}

	return    TxDataEvent; 
}

uint8_t  MasterRxErrHandle(void)
{
	bool	RXDataEvent=true;

	return    RXDataEvent; 
}

uint8_t  MasterRx01Handle(void)
{
	bool	RXDataEvent=true;

	return    RXDataEvent; 
}

uint8_t  MasterRx02Handle(void)
{
	bool	RXDataEvent=true;

	return    RXDataEvent; 
}

uint8_t  MasterRx03Handle(void)
{
	bool	RXDataEvent=true;

	return    RXDataEvent; 
}

uint8_t  MasterRx04Handle(void)
{
	bool	RXDataEvent=true;

	return    RXDataEvent; 
}

uint8_t  MasterRx05Handle(void)
{
	bool	RXDataEvent=true;

	return    RXDataEvent; 
}

uint8_t  MasterRx06Handle(void)
{
	bool	RXDataEvent=true;

	return    RXDataEvent; 
}

uint8_t  MasterRx0FHandle(void)
{
	bool	RXDataEvent=true;

	return    RXDataEvent; 
}

uint8_t  MasterRx10Handle(void)
{
	bool	RXDataEvent=true;

	return    RXDataEvent; 
}

