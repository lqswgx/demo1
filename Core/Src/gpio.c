/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
QueueHandle_t DI_Event_QueueHandle;
QueueHandle_t DO_Event_QueueHandle;
SemaphoreHandle_t DIDO_MutexSemHandle;

/**
 * @brief Digital Input Filter structure definition.
 * @details This structure is used to store the configuration of a digital input filter.
 *          It contains the GPIO port, pin, filter ticks, last raw state, last change tick, and stable state.
 *          The filter ticks is the time in ticks that the input must be stable before a change is considered.
 *          The last raw state is the last value read from the GPIO pin.
 *          The last change tick is the last time the input changed state.
 *          The stable state is the last stable value of the input.
 *          The filter ticks, last raw state, last change tick, and stable state are updated in the DI_Filter_Task thread.
 */
DigitalInputFilter di_channels[DI_CHANNEL_COUNT] =
{
    {KEY0_GPIO_Port, KEY0_Pin, 1, 1, 0, pdMS_TO_TICKS(20)},  // CH0: 20ms
    {KEY1_GPIO_Port, KEY1_Pin, 1, 1, 0, pdMS_TO_TICKS(20)},  // CH1: 50ms
    {KEY2_GPIO_Port, KEY2_Pin, 1, 1, 0, pdMS_TO_TICKS(20)},  // CH2: 10ms
    {KEY_UP_GPIO_Port, KEY_UP_Pin, 0, 0, 0, pdMS_TO_TICKS(30)}   // CH3: 30ms
};

/**
 * @brief Digital Output structure definition.
 * @details This structure is used to store the configuration of a digital output.
 *          It contains the GPIO port, pin, output mode, pulse frequency, current state, and last toggle tick.
 *          The output mode can be high, low, toggle, or pulse.
 *          The pulse frequency is the frequency of the pulse in Hz.
 *          The current state is the last value written to the GPIO pin.
 *          The last toggle tick is the last time the output was toggled.
 *          The output mode, pulse frequency, current state, and last toggle tick are updated in the DO_Control_Task thread.
 */
DigitalOutput_t do_channels[DO_CHANNEL_COUNT] =
{
    {LED0_GPIO_Port, LED0_Pin, OUTPUT_MODE_PULSE, 2, 0, 1},  // CH0:2Hz
    {LED1_GPIO_Port, LED1_Pin, OUTPUT_MODE_PULSE, 1, 0, 1},  // CH1:1Hz
};

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void MX_GPIO_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();		
    __HAL_RCC_GPIOH_CLK_ENABLE();		
    __HAL_RCC_GPIOI_CLK_ENABLE();
		
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(SPI5_NSS_GPIO_Port, SPI5_NSS_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOH, IIC_SCL_Pin|IIC_SDA_Pin, GPIO_PIN_SET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, LED1_Pin|LED0_Pin|LTDL_BL_Pin, GPIO_PIN_SET);

    /*Configure GPIO pins : KEY1_Pin KEY0_Pin */
    GPIO_InitStruct.Pin = KEY1_Pin|KEY0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /*Configure GPIO pin : KEY2_Pin */
    GPIO_InitStruct.Pin = KEY2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(KEY2_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : LED1_Pin LED0_Pin */
    GPIO_InitStruct.Pin = LED1_Pin|LED0_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /*Configure GPIO pin : LTDL_BL_Pin */
    GPIO_InitStruct.Pin = LTDL_BL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LTDL_BL_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */

/**
 * @brief Function implementing the DI_Filter_Task thread.
 * @param argument: Not used
 * @retval None
 * @note This function is called by the RTOS scheduler.
 */

void DI_Filter_Task(void const * argument)
{
    /* USER CODE BEGIN DI_Filter_Task */
    DI_Event_QueueHandle = xQueueCreate(DI_CHANNEL_COUNT, sizeof(DIDO_Event_t));
    DIDO_MutexSemHandle = xSemaphoreCreateMutex();

    TickType_t xLastWakeTime = xTaskGetTickCount();

    /* Infinite loop */
    for(;;)
    {
        for(uint8_t i=0; i<DI_CHANNEL_COUNT; i++)
        {
            DigitalInputFilter *ch = &di_channels[i];
            uint8_t current_state = HAL_GPIO_ReadPin(ch->port, ch->pin);

            if(current_state != ch->last_raw_state)
            {
                ch->last_raw_state = current_state;
                ch->last_change_tick = xTaskGetTickCount();
            }

            if((xTaskGetTickCount() - ch->last_change_tick) >= ch->filter_ticks)
            {
                if(ch->stable_state != current_state)
                {
                    ch->stable_state = current_state;

                    //DI事件通知
                    DIDO_Event_t event = {i, current_state};
                    xQueueSend(DI_Event_QueueHandle, &event, 0);
                }
            }
        }
        osDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));//1ms
//    osDelay(1);
    }
    /* USER CODE END DI_Filter_Task */
}

/**
 * @brief Function implementing the DO_Control_Task thread.
 * @param argument: Not used
 * @retval None
 * @note This function is called by the RTOS scheduler.
 */
void DO_Control_Task(void const * argument)
{
    /* USER CODE BEGIN DO_Control_Task */
    DI_Event_QueueHandle = xQueueCreate(DO_CHANNEL_COUNT, sizeof(DIDO_Event_t));

    TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t period = 0;
    /* Infinite loop */
    for(;;)
    {
        // Update DO channels
        for(uint8_t i=0; i<DO_CHANNEL_COUNT; i++)
        {
            DigitalOutput_t *ch = &do_channels[i];
            ch->last_state = ch->current_state;
            switch(ch->mode)
            {
                case OUTPUT_MODE_HIGH:
                    HAL_GPIO_WritePin(ch->port, ch->pin, GPIO_PIN_SET);
                    ch->current_state = 1;
                    break;

                case OUTPUT_MODE_LOW:
                    HAL_GPIO_WritePin(ch->port, ch->pin, GPIO_PIN_RESET);
                    ch->current_state = 0;
                    break;

                case OUTPUT_MODE_TOGGLE:
                    HAL_GPIO_TogglePin(ch->port, ch->pin);
                    ch->current_state = !ch->current_state;
                    ch->mode = (ch->current_state ? OUTPUT_MODE_HIGH: OUTPUT_MODE_LOW);
                    break;

                case OUTPUT_MODE_PULSE:
                    period = pdMS_TO_TICKS(1000 / ch->pulse_freq_hz);
                    if(xTaskGetTickCount() - ch->last_toggle_tick >= period)
                    {
                        HAL_GPIO_TogglePin(ch->port, ch->pin);
                        ch->current_state = !ch->current_state;
                        ch->last_toggle_tick = xTaskGetTickCount();
                    }
                    break;
            }

            if(ch->current_state != ch->last_state)
            {
                //DO事件通知   
                // DIDO_Event_t event = {i, ch->current_state};
                // xQueueSend(DO_Event_QueueHandle, &event, 0);
            }
        }

        // Wait for 5ms before checking again
        osDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
    }
    /* USER CODE END DO_Control_Task */
}

/**
 * @brief Function to safely set a DO channel output mode and frequency.
 * @param channel: The channel to set.
 * @param mode: The output mode to set.
 * @param freq: The pulse frequency to set.
 * @retval None
 * @note This function is thread-safe.
 * @note This function will block until the mutex is available.
 */
void SafeSetDO(uint8_t channel, OutputMode_t mode, uint32_t freq)
{
    if(channel >= DO_CHANNEL_COUNT) return;

    xSemaphoreTake(DIDO_MutexSemHandle, portMAX_DELAY);
    do_channels[channel].mode = mode;
    if(mode == OUTPUT_MODE_PULSE) {
        do_channels[channel].pulse_freq_hz = (freq > 1000) ? 1000 : freq;
    }
    xSemaphoreGive(DIDO_MutexSemHandle);
}

/**
 * @brief Function to safely get a DI channel input state.
 * @param channel: The channel to get.
 * @retval The input state of the channel.
 * @note This function is thread-safe.
 * @note This function will block until the mutex is available.
 * @note If the channel is not valid, the function will return 0xFF.
 */
uint8_t SafeGetDI(uint8_t channel)
{
    if(channel >= DI_CHANNEL_COUNT) return 0xFF;

    xSemaphoreTake(DIDO_MutexSemHandle, portMAX_DELAY);
    uint8_t state = di_channels[channel].stable_state;
    xSemaphoreGive(DIDO_MutexSemHandle);
    return state;
}

/* USER CODE END 2 */
