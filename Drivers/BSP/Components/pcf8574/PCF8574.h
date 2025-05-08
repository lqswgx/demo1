/*
 * @Author: LQS
 * @Date: 2025-04-15 18:31:53
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-25 16:47:16
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Drivers\BSP\Components\pcf8574\PCF8574.h
 * @Description: 
 */
#ifndef PCF8574_H_
#define PCF8574_H_

#include "main.h"
#include "iic.h"

#define PCF8574_Delay(x) delay_ms(x)

// #if DWT_EN
// #include "dwt.h"
// #define PCF8574_Delay(x) DWT_DelayMs(x)
// #else
// #define PCF8574_Delay(x) HAL_Delay(1)
// #endif

#define PCF8574_ADDR 0x40//地址
#define R_Bit 0x01 //读位
#define W_Bit 0x00 //写位

#define PCF8574_INT_Read() HAL_GPIO_ReadPin(PCF8574_INT_GPIO_Port, PCF8574_INT_Pin) //PCF8574中断引脚读取

//PCF8574各个IO的功能
#define BEEP_IO         0		//P0:蜂鸣器控制引脚
#define AP_INT_IO       1   	//P1:AP3216C中断引脚
#define DCMI_PWDN_IO    2    	//P2:DCMI的电源控制引脚
#define USB_PWR_IO      3    	//P3:USB电源控制引脚
#define EX_IO      		4    	//P4:扩展IO,自定义使用
#define MPU_INT_IO      5   	//P5:MPU9250中断引脚
#define RS485_RE_IO     6    	//P6:RS485_RE引脚
#define ETH_RESET_IO    7    	//P7:以太网复位引脚

void    PCF8574_Init(void);//初始化
void    PCF8574_WriteByte(uint8_t data); //写入数据
uint8_t PCF8574_ReadByte(void); //读取数据
void    PCF8574_WriteBit(uint8_t bit, uint8_t data); //写入指定位
uint8_t PCF8574_ReadBit(uint8_t bit); //读取指定位


void 	PHY_Reset(void);//PHY芯片复位
void    RS485_Dir_Set(uint8_t dir);//RS485方向设置

#endif /* PCF8574_H_ */
