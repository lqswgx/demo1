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
    if(HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR0) != 0x5550)//��һ������?
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

        HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0,0x5550);	//����Ѿ���ʼ������
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
 * @brief         : ��ʱ������룩ת����RTCʱ��
 * @date          : 2025-03-27
 * @param         : uint32_t stampTime ʱ������룩
 * @retval        : None
 */
void Set_TimeStamp_to_RtcTime(uint32_t stampTime)
{
    //������Ǳ���ʱ�䣨UTC+8����ʱ���������localtime��ȡ����UTCʱ�䣬���ȼ�8Сʱ
    time_t tick = (time_t)stampTime+8*3600;
    struct tm tt= *localtime(&tick);//

    SysTime.year		=	tt.tm_year+1900;
    SysTime.month		=	tt.tm_mon+1;
    SysTime.day			=	tt.tm_mday;
    SysTime.weekday	=	tt.tm_wday+1;
    SysTime.hours		=	tt.tm_hour;
    SysTime.minutes	=	tt.tm_min;
    SysTime.seconds	=	tt.tm_sec;

    Set_RTCTimeAndDate(&SysTime);//ͬ��RTCʱ��
}



/**
 * @name          : Get_RtcTime_to_TimeStamp
 * @brief         : ����ȡ��RTCʱ��ת����ʱ������룩
 * @date          : 2025-03-27
 * @param         : None
 * @retval        : uint32_t ʱ������룩
 */
uint32_t	Get_RtcTime_to_TimeStamp(void)
{
    uint32_t ret;
    struct tm tt;

    SysTime=Get_RTCTimeAndDate();//��ȡRTCʱ��

    tt.tm_year	=	SysTime.year-1900;
    tt.tm_mon		=	SysTime.month-1;
    tt.tm_mday	=	SysTime.day;
    tt.tm_wday	=	SysTime.weekday-1;
    tt.tm_hour	=	SysTime.hours;
    tt.tm_min		=	SysTime.minutes;
    tt.tm_sec		=	SysTime.seconds;
    tt.tm_isdst = -1;//�Զ��������ʱ

    ret	=	(uint32_t)mktime(&tt)-8*3600;

    return ret;
}


/**
 * @name          : Get_RTCTimeAndDate
 * @brief         : ��ȡRTCʱ��
 * @date          : 2025-03-27
 * @param         : None
 * @retval        : Time_Date_TypeDef �ṹ�壬��������������ʱ����
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
 * @brief         : ����RTCʱ��
 * @date          : 2025-03-27
 * @param         : Time_Date_TypeDef* rtc_time �ṹ�壬��������������ʱ����
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

//	����1
//	struct tm time_info={0};
//	time_t time_as_seconds=0;
//	time_info.tm_year	=	rtc_time->year-1900;//��1900�꿪ʼ����
//	time_info.tm_mon	=	rtc_time->month-1;//�·ݴ�0��ʼ
//	time_info.tm_mday	=	rtc_time->day;
//	time_info.tm_hour	=	rtc_time->hours;
//	time_info.tm_min	=	rtc_time->minutes;
//	time_info.tm_sec	=	rtc_time->seconds;
//	time_info.tm_isdst=	-1;//��mktime()�Զ��������ʱ
//	time_as_seconds		=	mktime(&time_info);//ת����time_t����,������ 1970 �� 1 �� 1 �� 00:00:00 ������������������,���ת��ʧ�ܣ��������� -1
//	sDate.WeekDay 		=	time_info.tm_wday+1;//1-7 ��һ������

//	����2
//	���û�ķ����ɭ��ʽ�������ڣ��ڹ�ʽ��d��ʾ�����е�������m��ʾ�·�����y��ʾ����
//	W= (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400) mod 7
//	ע�⣺�ڹ�ʽ���и���������ʽ��ͬ�ĵط���
//	��һ�ºͶ��¿�������һ���ʮ���º�ʮ���£����������2004-1-10����ɣ�2003-13-10�����빫ʽ���㡣�Թ�ԪԪ��Ϊ�ο�����ԪԪ��1��1��Ϊ����һ��
    uint16_t y=rtc_time->year,m=rtc_time->month,d=rtc_time->day;
    if(1==m || 2==m)m+=12,y--;
    RTC_DateStruct.WeekDay = (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400) % 7 + 1;//1-7 ��һ������

//	RTC_SetDate(RTC_Format_BIN,&RTC_DateStruct);
    if (HAL_RTC_SetDate(&hrtc, &RTC_DateStruct, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }

}

/* USER CODE END 1 */
