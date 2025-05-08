/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include "LogConfig.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <time.h>
#include <math.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

//GPIOx -> NUMx
#define GPIO_PIN_NUM(pin)  (uint8_t)(31 - __CLZ(pin))
		
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

//DI:KEY
#define KEY0_Pin                GPIO_PIN_3
#define KEY0_GPIO_Port 			GPIOH
#define KEY1_Pin                GPIO_PIN_2
#define KEY1_GPIO_Port 			GPIOH
#define KEY2_Pin                GPIO_PIN_13
#define KEY2_GPIO_Port 			GPIOC
#define KEY_UP_Pin              GPIO_PIN_0
#define KEY_UP_GPIO_Port 		GPIOA

//AI:TPAD
#define TPAD_Pin                GPIO_PIN_5
#define TPAD_GPIO_Port 			GPIOA

//LCD
#define LTDL_BL_Pin             GPIO_PIN_5
#define LTDL_BL_GPIO_Port 	    GPIOB

//DO:LED
#define LED0_Pin                GPIO_PIN_1
#define LED0_GPIO_Port 			GPIOB
#define LED1_Pin                GPIO_PIN_0
#define LED1_GPIO_Port 			GPIOB

//DHT11
#define DHT11_Pin               GPIO_PIN_4
#define DHT11_GPIO_Port 		GPIOA

//USART1:printf
#define USART1_TX_Pin 			GPIO_PIN_9
#define USART1_TX_GPIO_Port     GPIOA
#define USART1_RX_Pin 			GPIO_PIN_10
#define USART1_RX_GPIO_Port     GPIOA

//UART2:RS485/RS232
#define USART2_TX_Pin 			GPIO_PIN_2
#define USART2_TX_GPIO_Port     GPIOA
#define USART2_RX_Pin 			GPIO_PIN_3
#define USART2_RX_GPIO_Port     GPIOA

//UART3:RS232
#define USART3_TX_Pin 			GPIO_PIN_10
#define USART3_TX_GPIO_Port     GPIOB
#define USART3_RX_Pin 			GPIO_PIN_11
#define USART3_RX_GPIO_Port     GPIOB

//IIC:PCF8574/AT24C02
#define IIC_SCL_Pin             GPIO_PIN_4
#define IIC_SCL_GPIO_Port 	    GPIOH
#define IIC_SDA_Pin             GPIO_PIN_5
#define IIC_SDA_GPIO_Port 	    GPIOH
#define PCF8574_INT_Pin 		GPIO_PIN_12
#define PCF8574_INT_GPIO_Port   GPIOB

//CAN
#define CAN1_RX_Pin             GPIO_PIN_11
#define CAN1_RX_GPIO_Port 	    GPIOA
#define CAN1_TX_Pin             GPIO_PIN_12
#define CAN1_TX_GPIO_Port 	    GPIOA

//SPI5:W25Q256
#define SPI5_NSS_Pin 		    GPIO_PIN_6
#define SPI5_NSS_GPIO_Port 	    GPIOF
#define SPI5_SCK_Pin 		    GPIO_PIN_7
#define SPI5_SCK_GPIO_Port      GPIOF
#define SPI5_MISO_Pin 			GPIO_PIN_8
#define SPI5_MISO_GPIO_Port     GPIOF
#define SPI5_MOSI_Pin 			GPIO_PIN_9
#define SPI5_MOSI_GPIO_Port     GPIOF

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
