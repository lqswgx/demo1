/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sdio.h
  * @brief   This file contains all the function prototypes for
  *          the sdio.c file
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
#ifndef __SDIO_H__
#define __SDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "us_freertos.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern SD_HandleTypeDef hsd;
extern DMA_HandleTypeDef hdma_sdio_rx;
extern DMA_HandleTypeDef hdma_sdio_tx;

/* USER CODE BEGIN Private defines */
// SD 卡信息结构体
typedef struct {
  uint64_t capacity; // 卡容量（字节）
  uint32_t block_size; // 块大小（字节）
  uint32_t block_count; // 总块数
  char card_type[16]; // 卡类型（SDSC/SDHC/SDXC）
} SD_CardInfoTypeDef;
extern SD_CardInfoTypeDef SDCardInfo;
/* USER CODE END Private defines */

void MX_SDIO_SD_Init(void);
HAL_StatusTypeDef SD_GetCardInfo(SD_CardInfoTypeDef *info);
HAL_StatusTypeDef SD_WaitForTransferComplete(void);
HAL_StatusTypeDef SD_ReadBlocks(uint32_t blockAddr, uint8_t *buffer, uint32_t blockCount);
HAL_StatusTypeDef SD_WriteBlocks(uint32_t blockAddr, uint8_t *buffer, uint32_t blockCount);
HAL_StatusTypeDef SD_EraseBlocks(uint32_t startBlock, uint32_t endBlock);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __SDIO_H__ */

