/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.h
  * @brief   This file contains all the function prototypes for
  *          the rtc.c file
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
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern RTC_HandleTypeDef hrtc;

/**
 * @brief 	Time and Date structure definition
 * @note 	This structure is used to store the time and date information of the system.
 */
typedef struct
{
	uint8_t 	hours;
	uint8_t 	minutes;
	uint8_t 	seconds;
	uint16_t 	year;
	uint8_t		month;
	uint8_t 	day;
	uint8_t 	weekday;	
}Time_Date_TypeDef;

extern Time_Date_TypeDef SysTime;

void Set_TimeStamp_to_RtcTime(uint32_t stampTime);
uint32_t	Get_RtcTime_to_TimeStamp(void);

Time_Date_TypeDef Get_RTCTimeAndDate(void);
void Set_RTCTimeAndDate(Time_Date_TypeDef*	rtc_time);


/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_RTC_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __RTC_H__ */

