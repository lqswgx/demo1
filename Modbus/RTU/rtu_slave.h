/*
 * @Author: LQS
 * @Date: 2025-05-06 21:47:56
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 09:08:15
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\RTU\rtu_slave.h
 * @Description: 
 */
#ifndef __RTU_SLAVE_H__
#define __RTU_SLAVE_H__

#include  "main.h"  

typedef enum
{	
	RTU_SLAVE_STATE_READY=0,
	RTU_SLAVE_STATE_RX ,
	RTU_SLAVE_STATE_HANDLE,
	RTU_SLAVE_STATE_TX,
	RTU_SLAVE_STATE_TIMEOUT,
}RTU_SlaveStateTypeDef;
extern RTU_SlaveStateTypeDef  rtu_slave_state;

typedef struct
{
	uint8_t   rx_reflag;
	uint16_t  rx_count;
	uint8_t   tx_completed;
	uint16_t  tx_count;
	uint8_t   error_flag;
	uint8_t   error_code;
	uint8_t   error_cmd;
}RTU_SlaveHandleTypeDef;
extern RTU_SlaveHandleTypeDef rtu_slave;

#define  RTU_RX_BUFF_LEN_MAX        256
#define  RTU_TX_BUFF_LEN_MAX        1024

extern uint8_t  rtu_slave_rx_buff[RTU_RX_BUFF_LEN_MAX];
extern uint8_t  rtu_slave_tx_buff[RTU_TX_BUFF_LEN_MAX];

void rtu_slave_init(void);
void rtu_slave_poll(void);

void SlaveRtuRxIRQHandler(void);
void SlaveRtuTxIRQHandler(void);

#endif

