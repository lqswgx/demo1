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

//ϵͳ�������ģʽ
void Sys_Enter_Standby(void)
{
    __HAL_RCC_AHB1_FORCE_RESET();       //��λ����IO��

		if(SafeGetDI(DI_KEY_UP) == 1)return;
//    while(WKUP_KD);                     //�ȴ�WK_UP�����ɿ�(����RTC�ж�ʱ,�����WK_UP�ɿ��ٽ������)

    __HAL_RCC_PWR_CLK_ENABLE();         //ʹ��PWRʱ��
    __HAL_RCC_BACKUPRESET_FORCE();      //��λ��������
    HAL_PWR_EnableBkUpAccess();         //���������ʹ��

    //STM32F4,��������RTC����жϺ�,�����ȹر�RTC�ж�,�����жϱ�־λ,Ȼ����������
    //RTC�ж�,�ٽ������ģʽ�ſ�����������,�����������.
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);//�ر�RTCд����

    //�ر�RTC����жϣ�������RTCʵ�����
    __HAL_RTC_WAKEUPTIMER_DISABLE_IT(&hrtc,RTC_IT_WUT);
    __HAL_RTC_TIMESTAMP_DISABLE_IT(&hrtc,RTC_IT_TS);
    __HAL_RTC_ALARM_DISABLE_IT(&hrtc,RTC_IT_ALRA|RTC_IT_ALRB);

    //���RTC����жϱ�־λ
    __HAL_RTC_ALARM_CLEAR_FLAG(&hrtc,RTC_FLAG_ALRAF|RTC_FLAG_ALRBF);
    __HAL_RTC_TIMESTAMP_CLEAR_FLAG(&hrtc,RTC_FLAG_TSF);
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&hrtc,RTC_FLAG_WUTF);

    __HAL_RCC_BACKUPRESET_RELEASE();                    //��������λ����
    __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);     				//ʹ��RTCд����
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);                  //���Wake_UP��־

    HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1);           //����WKUP���ڻ��ѣ������ػ��ѣ�	
    HAL_PWR_EnterSTANDBYMode();                         //�������ģʽ
}

//WKUP���ѳ�ʼ��
void WKUP_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /*Configure GPIO pins : KEY_UP_Pin */
    GPIO_InitStruct.Pin	    =	KEY_UP_Pin;
    GPIO_InitStruct.Mode	=	GPIO_MODE_IT_RISING;//�����ؼ��
    GPIO_InitStruct.Pull	=	GPIO_PULLDOWN;
    HAL_GPIO_Init(KEY_UP_GPIO_Port,&GPIO_InitStruct);

//    //�ⲿ�ж�
//    HAL_NVIC_SetPriority(EXTI0_IRQn,5,0);
//    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

//�ⲿ�ж���0�жϷ�����
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(KEY_UP_Pin);
}

//�ж���0�жϴ������
//�˺����ᱻHAL_GPIO_EXTI_IRQHandler()����
//GPIO_Pin:����
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin==KEY_UP_Pin)//PA0
    {
//        if(Check_WKUP())//�ػ�
//        {
//            Sys_Enter_Standby();//�������ģʽ
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
        // KEY2�����󴥷�ϵͳ����;KEY_UP���»���ϵͳ
        if(SafeGetDI(DI_KEY2) == 0)
        {
            TickType_t press_start = xTaskGetTickCount();
            while(SafeGetDI(DI_KEY2) == 0) //�������
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



