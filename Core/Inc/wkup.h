/**
  ********************************** Copyright *********************************
  *
  ** (C) Copyright 2025 LQS,China.
  ** All Rights Reserved.
  *                              
  ******************************************************************************
  **--------------------------------------------------------------------------**
  ** @FileName      : wkup.h  
  ** @Description   : None
  **--------------------------------------------------------------------------**
  ** @Author        : LQS	  
  ** @Version       : v1.0				
  ** @Creat Date    : 2025-03-25  
  **--------------------------------------------------------------------------**
  ** @Attention     : None
  **--------------------------------------------------------------------------**
  ******************************************************************************
  *
 **/
 
/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __WKUP_H_
#define __WKUP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include "us_freertos.h"
#include "rtc.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define PRESS_DELAY_MAX	3000 //ms



void WKUP_Init(void);
void Sys_Enter_Standby(void);
/* USER CODE END Private defines */



/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif


/******************************** END OF FILE *********************************/


 

