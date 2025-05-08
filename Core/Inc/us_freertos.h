/**
  ********************************** Copyright *********************************
  *
  ** (C) Copyright 2025 LQS,China.
  ** All Rights Reserved.
  *                              
  ******************************************************************************
  **--------------------------------------------------------------------------**
  ** @FileName      : us_freertos.h  
  ** @Description   : None
  **--------------------------------------------------------------------------**
  ** @Author        : LQS	  
  ** @Version       : v1.0				
  ** @Creat Date    : 2025-03-25  
  **--------------------------------------------------------------------------**
  ** @Attention     : None
  **--------------------------------------------------------------------------**
  ******************************************************************************
  *
 **/
 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __US_FREERTOS_H_
#define __US_FREERTOS_H_

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "lwip.h"
#include "sdio.h"
#include "fatfs.h"
#include "AT_Cfg.h"

#include "gpio.h"
#include "iwdg.h"
#include "rtc.h"
#include "flash.h"

#include "w25qxx.h"
#include "PCF8574.h"
#include "lan8742.h"
#include "dth11.h"

#include "tcp_server.h"
#include "tcp_client.h"
#include "udp_server.h"
#include "udp_client.h"



extern	osThreadId DefaultTaskHandle;
extern	osThreadId DI_FILTER_TASKHANDLE;
extern	osThreadId DO_CONTROL_TASKHANDLE;
extern	osThreadId CAN1_TASKHandle;
extern	osThreadId USART1_TASKHandle;
extern	osThreadId USART2_TASKHandle;
extern	osThreadId USART3_TASKHandle;
extern	osThreadId SYSTEM_WAKEUP_TASKHandle;
extern	osThreadId IWDG_TASKHandle;
extern	osThreadId HEALTH_MONITOR_TASKHandle;
extern  osThreadId TCP_SERVER_TASKHandle;
extern  osThreadId TCP_CLIENT_TASKHandle;
extern  osThreadId UDP_SERVER_TASKHandle;
extern  osThreadId UDP_CLIENT_TASKHandle;
extern  osThreadId WIFI_STA_TASKHandle;
extern  osThreadId WIFI_AP_TASKHandle;
extern  osThreadId SD_CARD_TASKHandle;
extern  osThreadId MBRTU_SLAVE_TASKHandle;


void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

void StartDefaultTask(void const * argument);
void DI_Filter_Task(void const * argument);
void DO_Control_Task(void const * argument);
void Can1_Task(void const * argument);
void Usart1_Task(void const * argument);
void Usart2_Task(void const * argument);
void Uasrt3_Task(void const * argument);
void System_Wakeup_Task(void const * argument);
void Iwdg_Task(void const * argument);
void Health_Monitor_Task(void const * argument);
void Tcp_Server_Task(void const * argument);
void Tcp_Client_Task(void const * argument);
void Udp_Server_Task(void const * argument);
void Udp_Client_Task(void const * argument);
void Wifi_Sta_Task(void const * argument);
void Wifi_Ap_Task(void const * argument);
void Sd_Card_Task(void const * argument);
void Mbrtu_Slave_Task(void const * argument);


void Callback01(void const * argument);
void Callback02(void const * argument);
void Callback03(void const * argument);


#endif


/******************************** END OF FILE *********************************/


 

