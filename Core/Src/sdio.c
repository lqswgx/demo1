/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sdio.c
  * @brief   This file provides code for the configuration
  *          of the SDIO instances.
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
#include "sdio.h"
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;
SD_CardInfoTypeDef SDCardInfo;

#define SDIO_TIMEOUT	((uint32_t)0xFFFFF)
/* SDIO init function */

void MX_SDIO_SD_Init(void)
{

  /* USER CODE BEGIN SDIO_Init 0 */

  /* USER CODE END SDIO_Init 0 */

  /* USER CODE BEGIN SDIO_Init 1 */

  /* USER CODE END SDIO_Init 1 */
  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;
  /* USER CODE BEGIN SDIO_Init 2 */
#ifdef FatFS  
  if(HAL_SD_Init(&hsd)!= HAL_OK)
  {
    LOG_SDIO("SD Initialization Failed\n");
    Error_Handler();
  }
  if(HAL_SD_ConfigWideBusOperation(&hsd,SDIO_BUS_WIDE_4B)!=HAL_OK)
  {
    LOG_SDIO("Bus Width Configuration Failed\n");
    Error_Handler();
  } 
#endif  
  /* USER CODE END SDIO_Init 2 */

}

void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspInit 0 */

  /* USER CODE END SDIO_MspInit 0 */
    /* SDIO clock enable */
    __HAL_RCC_SDIO_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* SDIO DMA Init */
    /* SDIO_RX Init */
    hdma_sdio_rx.Instance = DMA2_Stream3;
    hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_rx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(sdHandle,hdmarx,hdma_sdio_rx);

    /* SDIO_TX Init */
    hdma_sdio_tx.Instance = DMA2_Stream6;
    hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_tx.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(sdHandle,hdmatx,hdma_sdio_tx);

    /* SDIO interrupt Init */
    HAL_NVIC_SetPriority(SDIO_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspInit 1 */

  /* USER CODE END SDIO_MspInit 1 */
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* sdHandle)
{

  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspDeInit 0 */

  /* USER CODE END SDIO_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();

    /**SDIO GPIO Configuration
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDIO DMA DeInit */
    HAL_DMA_DeInit(sdHandle->hdmarx);
    HAL_DMA_DeInit(sdHandle->hdmatx);

    /* SDIO interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspDeInit 1 */

  /* USER CODE END SDIO_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
// 获取 SD 卡信息
HAL_StatusTypeDef SD_GetCardInfo(SD_CardInfoTypeDef *info)
{
  HAL_SD_CardInfoTypeDef cardInfo;
  if (HAL_SD_GetCardInfo(&hsd, &cardInfo) != HAL_OK)
  {
      LOG_SDIO("Failed to Get Card Info\n");
      return HAL_ERROR;
  }

  // 填充信息
  info->block_size = cardInfo.BlockSize; // 通常 512 字节
  info->block_count = cardInfo.BlockNbr;
  info->capacity = (uint64_t)cardInfo.BlockSize * cardInfo.BlockNbr;

  // 判断卡类型
  switch (cardInfo.CardType)
  {
      case CARD_SDSC:
          strcpy(info->card_type, "SDSC");
          break;
      case CARD_SDHC_SDXC:
          strcpy(info->card_type, "SDHC/SDXC");
          break;
      default:
          strcpy(info->card_type, "Unknown");
          break;
  }

  return HAL_OK;
}

// 等待卡状态（非阻塞）
HAL_StatusTypeDef SD_WaitForTransferComplete(void)
{
    uint32_t tickstart = HAL_GetTick();
    
    while (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
	{
        if ((HAL_GetTick() - tickstart) >= SDIO_TIMEOUT)
		{
			LOG_SDIO("Timeout Waiting for Card State\n");
            return HAL_TIMEOUT;
        }
        vTaskDelay(1);
    }
    
    return HAL_OK;
}

// 读取多块数据
HAL_StatusTypeDef SD_ReadBlocks(uint32_t blockAddr, uint8_t *buffer, uint32_t blockCount)
{
    // 检查参数
    if (blockCount == 0 || buffer == NULL)
    {
        LOG_SDIO("Invalid Read Parameters\n");
        return HAL_ERROR;
    }

    // 限制最大块数（SD 卡规范建议最大 65535 块，STM32 SDIO 实际支持更少）
    if (blockCount > 65535)
    {
		LOG_SDIO("Too Many Blocks for Read\n");
        blockCount = 65535;
    }

    // 执行多块读取
    if (HAL_SD_ReadBlocks_DMA(&hsd, buffer, blockAddr, blockCount) != HAL_OK)
	{
        LOG_SDIO("Read Blocks Failed\n");
        return HAL_ERROR;
    }

    // 等待卡状态
    if (SD_WaitForTransferComplete() != HAL_OK)
	{
        LOG_SDIO("Read Card State Timeout\n");
        return HAL_TIMEOUT;
    }
  
  	return HAL_OK;
}

// 写入多块数据
HAL_StatusTypeDef SD_WriteBlocks(uint32_t blockAddr, uint8_t *buffer, uint32_t blockCount)
{
    // 检查参数
    if (blockCount == 0 || buffer == NULL)
    {
        LOG_SDIO("Invalid Write Parameters\n");
        return HAL_ERROR;
    }

    // 限制最大块数
    if (blockCount > 65535)
    {
        blockCount = 65535;
    }

    // 执行多块写入
    if (HAL_SD_WriteBlocks_DMA(&hsd, buffer, blockAddr, blockCount) != HAL_OK)
	{
        LOG_SDIO("Write Blocks Failed\n");
        return HAL_ERROR;
    }

    // 等待卡状态
    if (SD_WaitForTransferComplete() != HAL_OK)
	{
        LOG_SDIO("Write Card State Timeout\n");
        return HAL_TIMEOUT;
    }

  	return HAL_OK;
}

// 擦除指定块范围
HAL_StatusTypeDef SD_EraseBlocks(uint32_t startBlock, uint32_t endBlock)
{
    HAL_SD_CardInfoTypeDef cardInfo;

    // 获取卡信息以验证块范围
    if (HAL_SD_GetCardInfo(&hsd, &cardInfo) != HAL_OK)
    {
        LOG_SDIO("Failed to Get Card Info for Erase\n");
        return HAL_ERROR;
    }

    // 检查块地址有效性
    if (startBlock > endBlock || endBlock >= cardInfo.BlockNbr)
    {
        LOG_SDIO("Invalid Erase Block Range\n");
        return HAL_ERROR;
    }

    // 执行擦除
    if (HAL_SD_Erase(&hsd, startBlock, endBlock + 1) != HAL_OK)
	{
        LOG_SDIO("Erase Command Failed\n");
        return HAL_ERROR;
    }

    // 等待卡状态
    if (SD_WaitForTransferComplete() != HAL_OK)
	{
        LOG_SDIO("Erase Card State Timeout\n");
        return HAL_TIMEOUT;
    }

    return HAL_OK;
}

/* USER CODE END 1 */
