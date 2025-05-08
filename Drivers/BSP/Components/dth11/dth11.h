/*
 * @Author: LQS
 * @Date: 2025-04-24 12:00:22
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-24 17:26:09
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Drivers\BSP\Components\dth11\dth11.h
 * @Description: 
 */

#ifndef __DTH11_H_
#define __DTH11_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "dwt.h"

typedef struct
{
    uint8_t data[5];
    float temperature;
    float humidity;
}DHT11_Data_t;
extern DHT11_Data_t DHT11_Data;

// #define DHT11_Delay_us(x)      delay_us(x)
// #define DHT11_Delay_ms(x)      delay_ms(x)

#define DHT11_Delay_us(x)      DWT_DelayUs(x)
#define DHT11_Delay_ms(x)      DWT_DelayMs(x)

#define DHT11_PIN_H     HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_SET)
#define DHT11_PIN_L     HAL_GPIO_WritePin(DHT11_GPIO_Port, DHT11_Pin, GPIO_PIN_RESET)

#define DHT11_PIN_READ()  HAL_GPIO_ReadPin(DHT11_GPIO_Port, DHT11_Pin)

//DHT11输出模式初始化
#define DHT11_OUT_Init()  {DHT11_GPIO_Port->MODER &= ~(3 << (GPIO_PIN_NUM(DHT11_Pin) * 2));\
                            DHT11_GPIO_Port->MODER |= (1 << (GPIO_PIN_NUM(DHT11_Pin) * 2));}
//DHT11输入模式初始化
#define DHT11_IN_Init()   {DHT11_GPIO_Port->MODER &= ~(3 << (GPIO_PIN_NUM(DHT11_Pin) * 2));}

/* 函数声明 */
void DHT11_Init(void);
void DHT11_ReadData(DHT11_Data_t* dht11);

#ifdef __cplusplus
}
#endif

#endif




 

