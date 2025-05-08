/*
 * @Author: LQS
 * @Date: 2025-04-15 13:46:00
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-25 16:46:42
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Core\Inc\iic.h
 * @Description: 
 */
#ifndef __IIC_H__
#define __IIC_H__

#include "main.h"

#define IIC_Delay(x) delay_us(x)

// #define DWT_EN 1 //是否使用DWT

// #if DWT_EN
// #include "dwt.h"
// #define IIC_Delay(x) DWT_DelayUs(x)
// #else
// #define IIC_Delay(x) HAL_Delay(1)//最小1ms
// #endif

#define IIC_SCL_H HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, GPIO_PIN_SET) //SCL高
#define IIC_SCL_L HAL_GPIO_WritePin(IIC_SCL_GPIO_Port, IIC_SCL_Pin, GPIO_PIN_RESET) //SCL低

#define IIC_SDA_H HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_SET) //SDA高
#define IIC_SDA_L HAL_GPIO_WritePin(IIC_SDA_GPIO_Port, IIC_SDA_Pin, GPIO_PIN_RESET) //SDA低

#define IIC_SDA_READ() HAL_GPIO_ReadPin(IIC_SDA_GPIO_Port, IIC_SDA_Pin) //读取SDA状态

//SDA输出模式初始化
#define SDA_OUT_Init()  {IIC_SDA_GPIO_Port->MODER &= ~(3 << (GPIO_PIN_NUM(IIC_SDA_Pin) * 2));\
                         IIC_SDA_GPIO_Port->MODER |= (1 << (GPIO_PIN_NUM(IIC_SDA_Pin) * 2));}
//SDA输入模式初始化
#define SDA_IN_Init()   {IIC_SDA_GPIO_Port->MODER &= ~(3 << (GPIO_PIN_NUM(IIC_SDA_Pin) * 2));}

void    IIC_Init(void);
void    IIC_Start(void);
void    IIC_Stop(void);
void    IIC_SendByte(uint8_t byte);
uint8_t IIC_ReadByte(void);
uint8_t IIC_WaitAck(void);
void    IIC_Ack(void);
void    IIC_NAck(void);


#endif /* __IIC_H__ */
