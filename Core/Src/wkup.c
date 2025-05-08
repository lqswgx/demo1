/**
  ********************************** Copyright *********************************
  *
  ** (C) Copyright 2025 LQS,China.
  ** All Rights Reserved.
  *
  ******************************************************************************
  **--------------------------------------------------------------------------**
  ** @FileName      : wkup.c
  ** @Brief         : None
  **--------------------------------------------------------------------------**
  ** @Author Data   : LQS 2025-03-25
  **--------------------------------------------------------------------------**
  ** @Attention     : None
  **--------------------------------------------------------------------------**
  ******************************************************************************
  *
 **/
#include "wkup.h"

//系统进入待机模式
void Sys_Enter_Standby(void)
{
    __HAL_RCC_AHB1_FORCE_RESET();       //复位所有IO口

		if(SafeGetDI(DI_KEY_UP) == 1)return;
//    while(WKUP_KD);                     //等待WK_UP按键松开(在有RTC中断时,必须等WK_UP松开再进入待机)

    __HAL_RCC_PWR_CLK_ENABLE();         //使能PWR时钟
    __HAL_RCC_BACKUPRESET_FORCE();      //复位备份区域
    HAL_PWR_EnableBkUpAccess();         //后备区域访问使能

    //STM32F4,当开启了RTC相关中断后,必须先关闭RTC中断,再清中断标志位,然后重新设置
    //RTC中断,再进入待机模式才可以正常唤醒,否则会有问题.
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);//关闭RTC写保护

    //关闭RTC相关中断，可能在RTC实验打开了
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&hrtc,RTC_IT_WUT);
    __HAL_RTC_TIMESTAMP_DISABLE_IT(&hrtc,RTC_IT_TS);
    __HAL_RTC_ALARM_DISABLE_IT(&hrtc,RTC_IT_ALRA|RTC_IT_ALRB);

    //清除RTC相关中断标志位
    __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc,RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
    __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&hrtc,RTC_FLAG_TSF);
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc,RTC_FLAG_WUTF);

    __HAL_RCC_BACKUPRESET_RELEASE();                    //备份区域复位结束
    __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);     				//使能RTC写保护
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //清除Wake_UP标志

    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           //设置WKUP用于唤醒（上升沿唤醒）	
    HAL_PWR_EnterSTANDBYMode();                         //进入待机模式
}

//WKUP唤醒初始化
void WKUP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pins : KEY_UP_Pin */
    GPIO_InitStruct.Pin	    =	KEY_UP_Pin;
    GPIO_InitStruct.Mode	=	GPIO_MODE_IT_RISING;//上升沿检测
    GPIO_InitStruct.Pull	=	GPIO_PULLDOWN;
    HAL_GPIO_Init(KEY_UP_GPIO_Port,&GPIO_InitStruct);

//    //外部中断
//    HAL_NVIC_SetPriority(EXTI0_IRQn,5,0);
//    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

//外部中断线0中断服务函数
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_UP_Pin);
}

//中断线0中断处理过程
//此函数会被HAL_GPIO_EXTI_IRQHandler()调用
//GPIO_Pin:引脚
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==KEY_UP_Pin)//PA0
    {
//        if(Check_WKUP())//关机
//        {
//            Sys_Enter_Standby();//进入待机模式
//        }
    }
}

/* USER CODE BEGIN Header_System_Wakeup_Task */
/**
* @brief Function implementing the SYSTEM_WAKEUP_T thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_System_Wakeup_Task */
void System_Wakeup_Task(void const * argument)
{
    /* USER CODE BEGIN System_Wakeup_Task */
    /* Infinite loop */
    for(;;)
    {
        // KEY2长按后触发系统待机;KEY_UP按下唤醒系统
        if(SafeGetDI(DI_KEY2) == 0)
        {
            TickType_t press_start = xTaskGetTickCount();
            while(SafeGetDI(DI_KEY2) == 0) //持续检测
            {
                if(xTaskGetTickCount() - press_start > pdMS_TO_TICKS(PRESS_DELAY_MAX))
                {
                    Sys_Enter_Standby();
                    break;
                }
                osDelay(10);
            }
        }
				
        osDelay(1000);
    }
    /* USER CODE END System_Wakeup_Task */
}



