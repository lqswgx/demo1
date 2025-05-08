/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "us_freertos.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
extern QueueHandle_t DI_Event_QueueHandle;
extern QueueHandle_t DO_Event_QueueHandle;

#define DI_CHANNEL_COUNT  4  // DI conunt
#define DO_CHANNEL_COUNT  2  // Do conunt

/**
 * @brief DI channel definition
 * @note The DI channel definition is used to identify the DI channel in the code.
 */
typedef enum
{
	DI_KEY0,
	DI_KEY1,
	DI_KEY2,
	DI_KEY_UP
}DiChannel_t;


/**
 * @brief DO channel definition
 * @note The DO channel definition is used to identify the DO channel in the code.
 */
typedef enum
{
	DO_LED0,
	DO_LED1,
}DoChannel_t;


typedef struct 
{
  GPIO_TypeDef *port;        		// GPIO port
  uint16_t pin;              		// GPIO pin
  bool stable_state;      			// Stable state of the input signal
  bool last_raw_state;    			// Last raw state of the input signal
  TickType_t last_change_tick; 	// Last change tick of the input signal
  TickType_t filter_ticks;   		// Filter ticks for debounce
} DigitalInputFilter;

extern DigitalInputFilter di_channels[DI_CHANNEL_COUNT];


typedef struct 
{
  uint8_t channel;    // DI channel
  bool new_state;     // New state of the input signal
} DIDO_Event_t;


typedef enum 
{
  OUTPUT_MODE_HIGH,      // Output high
  OUTPUT_MODE_LOW,       // Output low
  OUTPUT_MODE_TOGGLE,    // Toggle output
  OUTPUT_MODE_PULSE      // Pulse output
} OutputMode_t;


typedef struct 
{
	GPIO_TypeDef *port; 			 	// GPIO port
  uint16_t pin;              	// GPIO pin
  OutputMode_t mode;         	// Output mode
  uint32_t pulse_freq_hz;    	// Pulse frequency in Hz
  TickType_t last_toggle_tick; // Last toggle tick of the output signal
  bool last_state;          	// Last state of the output signal
  bool current_state;     		// Current state of the output signal
} DigitalOutput_t;

extern DigitalOutput_t do_channels[DO_CHANNEL_COUNT];

/* USER CODE END Private defines */

void MX_GPIO_Init(void);
void SafeSetDO(uint8_t channel, OutputMode_t mode, uint32_t freq);
uint8_t SafeGetDI(uint8_t channel);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

