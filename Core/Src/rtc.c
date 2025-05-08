/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    rtc.c
  * @brief   This file provides code for the configuration
  *          of the RTC instances.
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
/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;
Time_Date_TypeDef SysTime;
/* RTC init function */
void MX_RTC_Init(void)
{
    /* USER CODE BEGIN RTC_Init 0 */

    /** Initialize RTC Only
    */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE END RTC_Init 0 */

    /* Configure RTC peripheral according to your requirements here */
    if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x5550)//第一次配置?
    {
        RTC_TimeTypeDef sTime = {0};
        RTC_DateTypeDef sDate = {0};

        sTime.Hours = 0x12;
        sTime.Minutes = 0x20;
        sTime.Seconds = 0x00;
        sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTime.StoreOperation = RTC_STOREOPERATION_RESET;
        if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
        {
            Error_Handler();
        }
        sDate.WeekDay = RTC_WEEKDAY_THURSDAY;
        sDate.Month = RTC_MONTH_MARCH;
        sDate.Date = 0x27;
        sDate.Year = 0x25;

        if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
        {
            Error_Handler();
        }

        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x5550);	//标记已经初始化过了
    }

    /* USER CODE BEGIN RTC_Init 2 */

    /* USER CODE END RTC_Init 2 */

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if(rtcHandle->Instance==RTC)
    {
        /* USER CODE BEGIN RTC_MspInit 0 */

        /* USER CODE END RTC_MspInit 0 */

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
        PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        /* RTC clock enable */
        __HAL_RCC_RTC_ENABLE();
        /* USER CODE BEGIN RTC_MspInit 1 */

        /* USER CODE END RTC_MspInit 1 */
    }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

    if(rtcHandle->Instance==RTC)
    {
        /* USER CODE BEGIN RTC_MspDeInit 0 */

        /* USER CODE END RTC_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_RTC_DISABLE();
        /* USER CODE BEGIN RTC_MspDeInit 1 */

        /* USER CODE END RTC_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/**
 * @name          : Set_TimeStamp_to_RtcTime
 * @brief         : 将时间戳（秒）转换成RTC时间
 * @date          : 2025-03-27
 * @param         : uint32_t stampTime 时间戳（秒）
 * @retval        : None
 */
void Set_TimeStamp_to_RtcTime(uint32_t stampTime)
{
    //输入的是北京时间（UTC+8）的时间戳，由于localtime获取的是UTC时间，则先加8小时
    time_t tick = (time_t)stampTime+8*3600;
    struct tm tt= *localtime(&tick);//

    SysTime.year		=	tt.tm_year+1900;
    SysTime.month		=	tt.tm_mon+1;
    SysTime.day			=	tt.tm_mday;
    SysTime.weekday	=	tt.tm_wday+1;
    SysTime.hours		=	tt.tm_hour;
    SysTime.minutes	=	tt.tm_min;
    SysTime.seconds	=	tt.tm_sec;

    Set_RTCTimeAndDate(&SysTime);//同步RTC时间
}



/**
 * @name          : Get_RtcTime_to_TimeStamp
 * @brief         : 将获取的RTC时间转换成时间戳（秒）
 * @date          : 2025-03-27
 * @param         : None
 * @retval        : uint32_t 时间戳（秒）
 */
uint32_t	Get_RtcTime_to_TimeStamp(void)
{
    uint32_t ret;
    struct tm tt;

    SysTime=Get_RTCTimeAndDate();//读取RTC时间

    tt.tm_year	=	SysTime.year-1900;
    tt.tm_mon		=	SysTime.month-1;
    tt.tm_mday	=	SysTime.day;
    tt.tm_wday	=	SysTime.weekday-1;
    tt.tm_hour	=	SysTime.hours;
    tt.tm_min		=	SysTime.minutes;
    tt.tm_sec		=	SysTime.seconds;
    tt.tm_isdst = -1;//自动检测夏令时

    ret	=	(uint32_t)mktime(&tt)-8*3600;

    return ret;
}


/**
 * @name          : Get_RTCTimeAndDate
 * @brief         : 获取RTC时间
 * @date          : 2025-03-27
 * @param         : None
 * @retval        : Time_Date_TypeDef 结构体，包含年月日星期时分秒
 */
Time_Date_TypeDef Get_RTCTimeAndDate(void)
{
    Time_Date_TypeDef rtc_time;
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;
//  RTC_GetTime(RTC_Format_BIN,	&RTC_TimeStruct);
//  RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

    if (HAL_RTC_GetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_RTC_GetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }

    rtc_time.year			=	RTC_DateStruct.Year+2000;
    rtc_time.month		=	RTC_DateStruct.Month;
    rtc_time.day			=	RTC_DateStruct.Date;
    rtc_time.weekday	=	RTC_DateStruct.WeekDay;
    rtc_time.hours		=	RTC_TimeStruct.Hours;
    rtc_time.minutes	=	RTC_TimeStruct.Minutes;
    rtc_time.seconds	=	RTC_TimeStruct.Seconds;

    return rtc_time;
}


/**
 * @name          : Set_RTCTimeAndDate
 * @brief         : 设置RTC时间
 * @date          : 2025-03-27
 * @param         : Time_Date_TypeDef* rtc_time 结构体，包含年月日星期时分秒
 * @retval        : None
 */
void Set_RTCTimeAndDate(Time_Date_TypeDef*	rtc_time)
{
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    RTC_TimeStruct.Hours			=	rtc_time->hours;
    RTC_TimeStruct.Minutes		=	rtc_time->minutes;
    RTC_TimeStruct.Seconds		=	rtc_time->seconds;
    RTC_TimeStruct.TimeFormat	=	RTC_HOURFORMAT12_AM;//AM
//	RTC_SetTime(RTC_Format_BIN,&RTC_TimeStruct);
    if (HAL_RTC_SetTime(&hrtc, &RTC_TimeStruct, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }

    RTC_DateStruct.Year		=	rtc_time->year-2000;
    RTC_DateStruct.Month	=	rtc_time->month;
    RTC_DateStruct.Date		=	rtc_time->day;

//	方法1
//	struct tm time_info={0};
//	time_t time_as_seconds=0;
//	time_info.tm_year	=	rtc_time->year-1900;//从1900年开始计算
//	time_info.tm_mon	=	rtc_time->month-1;//月份从0开始
//	time_info.tm_mday	=	rtc_time->day;
//	time_info.tm_hour	=	rtc_time->hours;
//	time_info.tm_min	=	rtc_time->minutes;
//	time_info.tm_sec	=	rtc_time->seconds;
//	time_info.tm_isdst=	-1;//让mktime()自动检测夏令时
//	time_as_seconds		=	mktime(&time_info);//转换成time_t类型,返回自 1970 年 1 月 1 日 00:00:00 到现在所经过的秒数,如果转换失败，函数返回 -1
//	sDate.WeekDay 		=	time_info.tm_wday+1;//1-7 周一到周日

//	方法2
//	利用基姆拉尔森公式计算星期，在公式中d表示日期中的日数，m表示月份数，y表示年数
//	W= (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400) mod 7
//	注意：在公式中有个与其他公式不同的地方：
//	把一月和二月看成是上一年的十三月和十四月，例：如果是2004-1-10则换算成：2003-13-10来代入公式计算。以公元元年为参考，公元元年1月1日为星期一。
    uint16_t y=rtc_time->year,m=rtc_time->month,d=rtc_time->day;
    if(1==m || 2==m)m+=12,y--;
    RTC_DateStruct.WeekDay = (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400) % 7 + 1;//1-7 周一到周日

//	RTC_SetDate(RTC_Format_BIN,&RTC_DateStruct);
    if (HAL_RTC_SetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }

}

/* USER CODE END 1 */
