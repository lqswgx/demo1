/*
 * @Author: LQS
 * @Date: 2025-04-25 15:28:38
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-25 16:37:16
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Core\Src\delay.c
 * @Description: 
 */
#include "delay.h"

// 每微秒的循环次数（需根据实际主频校准）
static uint32_t us_ticks = 0;

// 初始化延时，传入系统时钟频率（单位：MHz）
void delay_init(void) 
{
    // 假设每循环消耗约4个时钟周期（基于Cortex-M4指令特性）
    // 每微秒的循环次数 = 系统时钟频率（Hz） / 4 / 1,000,000
    us_ticks = HAL_RCC_GetSysClockFreq() / 4 / 1000000;
}

// 微秒延时
void delay_us(uint32_t us) 
{
    uint32_t loops = us * us_ticks;
    while (loops--) 
    {
        __NOP(); // 空操作，确保编译器不优化掉
    }
}

// 毫秒延时
void delay_ms(uint32_t ms) 
{
    while (ms--) 
    {
        delay_us(1000); // 1ms = 1000us
    }
}

