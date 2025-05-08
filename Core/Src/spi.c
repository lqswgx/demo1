/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.c
  * @brief   This file provides code for the configuration
  *          of the SPI instances.
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
#include "spi.h"

/* USER CODE BEGIN 0 */
#define SPI_MODE    3 //SPIģʽ

/* USER CODE END 0 */

SPI_HandleTypeDef hspi5;

/* SPI5 init function */
void MX_SPI5_Init(void)
{

    /* USER CODE BEGIN SPI5_Init 0 */

    /* USER CODE END SPI5_Init 0 */

    /* USER CODE BEGIN SPI5_Init 1 */

    /* USER CODE END SPI5_Init 1 */
    hspi5.Instance = SPI5;
    hspi5.Init.Mode = SPI_MODE_MASTER;
    hspi5.Init.Direction = SPI_DIRECTION_2LINES;
    hspi5.Init.DataSize = SPI_DATASIZE_8BIT;

#if	    SPI_MODE == 0
		hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;	
		
#elif   SPI_MODE == 1
		hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
		hspi5.Init.CLKPhase = SPI_PHASE_2EDGE;
		
#elif   SPI_MODE == 2
		hspi5.Init.CLKPolarity = SPI_POLARITY_HIGH;
		hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
		
#elif   SPI_MODE == 3
		hspi5.Init.CLKPolarity = SPI_POLARITY_HIGH;
		hspi5.Init.CLKPhase = SPI_PHASE_2EDGE;		
#endif

    hspi5.Init.NSS = SPI_NSS_SOFT;
    hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi5.Init.CRCPolynomial = 10;
    if (HAL_SPI_Init(&hspi5) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN SPI5_Init 2 */
		
	SPI_ReadWriteByte(0xff);
		
    /* USER CODE END SPI5_Init 2 */

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(spiHandle->Instance==SPI5)
    {
        /* USER CODE BEGIN SPI5_MspInit 0 */

        /* USER CODE END SPI5_MspInit 0 */
        /* SPI5 clock enable */
        __HAL_RCC_SPI5_CLK_ENABLE();

        __HAL_RCC_GPIOF_CLK_ENABLE();
        /**SPI5 GPIO Configuration
        PF7     ------> SPI5_SCK
        PF8     ------> SPI5_MISO
        PF9     ------> SPI5_MOSI
        */
        GPIO_InitStruct.Pin = SPI5_SCK_Pin|SPI5_MISO_Pin|SPI5_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        /*Configure GPIO pin : SPI5_NSS_Pin */
        GPIO_InitStruct.Pin = SPI5_NSS_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(SPI5_NSS_GPIO_Port, &GPIO_InitStruct);
        			
        /* USER CODE BEGIN SPI5_MspInit 1 */

        /* USER CODE END SPI5_MspInit 1 */
    }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

    if(spiHandle->Instance==SPI5)
    {
        /* USER CODE BEGIN SPI5_MspDeInit 0 */

        /* USER CODE END SPI5_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI5_CLK_DISABLE();

        /**SPI5 GPIO Configuration
        PF7     ------> SPI5_SCK
        PF8     ------> SPI5_MISO
        PF9     ------> SPI5_MOSI
        */
        HAL_GPIO_DeInit(GPIOF, SPI5_SCK_Pin|SPI5_MISO_Pin|SPI5_MOSI_Pin);

        /* USER CODE BEGIN SPI5_MspDeInit 1 */

        /* USER CODE END SPI5_MspDeInit 1 */
    }
}

/* USER CODE BEGIN 1 */

/**
 * @brief  SPI5��дһ���ֽ�����
 * @param  TxData:�����͵�����
 * @return �յ�������
 */
uint8_t SPI_ReadWriteByte(uint8_t TxData)
{
	uint8_t Rxdata=0;
	
	HAL_SPI_TransmitReceive(&hspi5,&TxData,&Rxdata,1, 1000);  
	
	return Rxdata;		
}
/* USER CODE END 1 */
