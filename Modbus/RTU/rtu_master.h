/*
 * @Author: LQS
 * @Date: 2025-05-06 21:47:56
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-08 17:56:20
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\RTU\rtu_master.h
 * @Description: 
 */
#ifndef __RTU_MASTER_H__
#define __RTU_MASTER_H__

#include  "main.h"  

#define   MasterTXBuffLenMax	256U
#define   MasterRXBuffLenMax	1024U

extern uint8_t MasterRxBuffTable[MasterRXBuffLenMax];
extern uint8_t MasterTxBuffTable[MasterTXBuffLenMax];

typedef enum
{
	ExecuteDisabled=0,
	ExecuteEnabled
}RTU_MasterExecuteStateTypeDef;

typedef enum
{	
	RTU_MASTER_STATE_READY=0,
	RTU_MASTER_STATE_RX,
	RTU_MASTER_STATE_HANDLE,
	RTU_MASTER_STATE_TX,
	RTU_MASTER_STATE_TIMEOUT,
	RTU_MASTER_STATE_COMPLETED,
}RTU_MasterStateTypeDef;
extern RTU_MasterStateTypeDef    RTU_MASTER_STATE;

typedef struct
{
	uint8_t     rx_reflag;
	uint16_t    rx_datacount;
	uint16_t    rx_time_out;
	uint8_t     rx_errorcount;
	uint8_t     tx_completed;
	uint16_t    tx_count;
	uint8_t     rx_func_code_err;
}RTU_MasterHandleTypeDef;
extern RTU_MasterHandleTypeDef   rtu_master;

#define RTU_MASTER_TX_REG_BYTES  32U
#define RTU_MASTER_TX_COIL_BYTES  8U
typedef struct
{
	uint8_t   slave_addr; 
	uint8_t   func_code;     
	uint8_t	  start_addr;
	bool   	  bool_val;     
	uint16_t  data_num;
	uint8_t   data_bytes;      
	uint16_t  reg_val[RTU_MASTER_TX_REG_BYTES];
	uint8_t   coil_val[RTU_MASTER_TX_COIL_BYTES];
	uint8_t   ExecutionEventsState;
}MBRTUFormatTypeDef;
extern MBRTUFormatTypeDef MasterTxFormat;

void rtu_master_init(void);
void rtu_master_poll(void);

void MasterRtuRxIRQHandler(void);
void MasterRtuTxIRQHandler(void);

uint8_t  MasterTxDataHandle(void);


typedef uint8_t (*MasterRxHandleFunc)(void);
extern MasterRxHandleFunc master_rx_handle[17];

uint8_t  MasterRxErrHandle(void);
uint8_t  MasterRx01Handle(void);
uint8_t  MasterRx02Handle(void);
uint8_t  MasterRx03Handle(void);
uint8_t  MasterRx04Handle(void);
uint8_t  MasterRx05Handle(void);
uint8_t  MasterRx06Handle(void);
uint8_t  MasterRx0FHandle(void);
uint8_t  MasterRx10Handle(void);

#endif
