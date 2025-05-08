/*
 * @Author: LQS
 * @Date: 2025-04-15 18:03:15
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-21 13:35:40
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\LWIP\App\lwip.h
 * @Description: 
 */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : LWIP.h
  * Description        : This file provides code for the configuration
  *                      of the LWIP.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *************************************************************************

  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __mx_lwip_H
#define __mx_lwip_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "ethernetif.h"

/* Includes for RTOS ---------------------------------------------------------*/
#if WITH_RTOS
#include "lwip/tcpip.h"
#include "lwip/udp.h"
#endif /* WITH_RTOS */

/* USER CODE BEGIN 0 */
/* Define the  REMOTE IP ADDRESS AND PORT  */
#define REMOTE_IP_ADDR1 192
#define REMOTE_IP_ADDR2 168
#define REMOTE_IP_ADDR3 1
#define REMOTE_IP_ADDR4 200
#define REMOTE_PORT     8080
/* Define the LOCAL IP address and port of the board */
#define LOCAL_IP_ADDRESS1		192
#define LOCAL_IP_ADDRESS2		168
#define LOCAL_IP_ADDRESS3		1
#define LOCAL_IP_ADDRESS4		100
#define LOCAL_PORT          8080 
/* Define the netmask of the board */
#define NETMASK_ADDR1  255
#define NETMASK_ADDR2  255
#define NETMASK_ADDR3  255
#define NETMASK_ADDR4  0
/* Define the gateway of the board */
#define GATEWAY_ADDR1  192
#define GATEWAY_ADDR2  168
#define GATEWAY_ADDR3  1
#define GATEWAY_ADDR4  1
/* USER CODE END 0 */

/* Global Variables ----------------------------------------------------------*/
extern ETH_HandleTypeDef heth;

/* LWIP init function */
void MX_LWIP_Init(void);

#if !WITH_RTOS
/* USER CODE BEGIN 1 */
/* Function defined in lwip.c to:
 *   - Read a received packet from the Ethernet buffers
 *   - Send it to the lwIP stack for handling
 *   - Handle timeouts if NO_SYS_NO_TIMERS not set
 */
void MX_LWIP_Process(void);

/* USER CODE END 1 */
#endif /* WITH_RTOS */

#ifdef __cplusplus
}
#endif
#endif /*__ mx_lwip_H */

/**
  * @}
  */

/**
  * @}
  */
