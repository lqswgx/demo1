/*
 * @Author: LQS
 * @Date: 2025-04-24 10:18:31
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-28 21:45:31
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Core\Src\dwt.c
 * @Description: 
 */
#include "dwt.h"

/**
  * @brief  Initializes DWT timer
  * @retval None
  */
void MX_DWT_Init(void)
{
  volatile uint32_t delay = 1000000;  // 短暂延迟等待调试器完成连接
  while(delay--);
    
  CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk;// Enable tracing
  DWT->CYCCNT = 0UL;// Reset cycle counter
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk;// Enable cycle counter

  if(DWT->CYCCNT)
  {
    LOG_DWT("DWT timer is working\n");
  }
  else
  {
    LOG_DWT("DWT timer is not working\n");
  }
}

/**
  * @brief  Returns current DWT counter value
  * @retval Current DWT counter value
  */
uint32_t DWT_GetTicks(void)
{
    return DWT->CYCCNT;
}

/**
  * @brief  Converts DWT ticks to microseconds
  * @param  ticks: DWT ticks to convert
  * @retval Converted microseconds
  */
uint32_t DWT_TicksToUs(uint32_t ticks)
{
    // Assuming 180MHz clock (STM32F429 typical frequency)
    // 180 ticks = 1us
    return ticks / (SystemCoreClock / 1000000);
}

/**
  * @brief  Converts DWT ticks to milliseconds
  * @param  ticks: DWT ticks to convert
  * @retval Converted milliseconds
  */
uint32_t DWT_TicksToMs(uint32_t ticks)
{
    // 180,000 ticks = 1ms at 180MHz
    return ticks / (SystemCoreClock / 1000);
}

/**
  * @brief  Delays execution for specified microseconds
  * @param  us: microseconds to delay
  */
void DWT_DelayUs(uint32_t us)
{
    uint32_t startTick = DWT->CYCCNT;
    uint32_t delayTicks = us * (SystemCoreClock / 1000000);
    
    while((DWT->CYCCNT - startTick) < delayTicks);
}

/**
  * @brief  Delays execution for specified milliseconds
  * @param  ms: milliseconds to delay
  */
void DWT_DelayMs(uint32_t ms)
{
    DWT_DelayUs(ms * 1000);
}


