/*
 * @Author: LQS
 * @Date: 2025-04-24 10:18:39
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-24 11:03:23
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Core\Src\dwt.h
 * @Description: 
 */
#ifndef __DWT_H
#define __DWT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// DWT control register macros
#define DWT_CR      *(volatile uint32_t*)0xE0001000
#define DWT_CYCCNT  *(volatile uint32_t*)0xE0001004
#define DEM_CR      *(volatile uint32_t*)0xE000EDFC

// DEM_CR bit definitions
#define DEM_CR_TRCENA   (1 << 24)

// DWT_CR bit definitions
#define DWT_CR_CYCCNTENA (1 << 0)

/**
  * @brief  Initializes DWT timer
  * @retval 0 if initialization failed, 1 if succeeded
  */
void MX_DWT_Init(void);

/**
  * @brief  Returns current DWT counter value
  * @retval Current DWT counter value
  */
uint32_t DWT_GetTicks(void);

/**
  * @brief  Converts DWT ticks to microseconds
  * @param  ticks: DWT ticks to convert
  * @retval Converted microseconds
  */
uint32_t DWT_TicksToUs(uint32_t ticks);

/**
  * @brief  Converts DWT ticks to milliseconds
  * @param  ticks: DWT ticks to convert
  * @retval Converted milliseconds
  */
uint32_t DWT_TicksToMs(uint32_t ticks);

/**
  * @brief  Delays execution for specified microseconds
  * @param  us: microseconds to delay
  */
void DWT_DelayUs(uint32_t us);

/**
  * @brief  Delays execution for specified milliseconds
  * @param  ms: milliseconds to delay
  */
void DWT_DelayMs(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* DWT_H */
