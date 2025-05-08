/**
  * @file    w25qxx.h
  * @brief   This file contains all the functions prototypes for the W25Qxx series FLASH memory.
  * @version V1.0
  * @date    2025-03-27
  * @author  LQS
  * @note    This file is the header file for w25qxx.c driver.
  *          It includes the necessary functions prototypes for 
  *          the usage of W25Qxx series FLASH memory.
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __W25QXX_H_
#define __W25QXX_H_

#include "main.h"
#include "spi.h"
#include "us_freertos.h"

typedef enum
{
    W25Q80   = 0XEF13U,
    W25Q16   = 0XEF14U,
    W25Q32   = 0XEF15U,
    W25Q64   = 0XEF16U,
    W25Q128  = 0XEF17U,
    W25Q256  = 0XEF18U,
    W25Q512  = 0XEF19U,
    W25Q01   = 0XEF20U,
    W25Q02   = 0XEF21U,
} W25Qxx_ID_TypeDef;


/* Reset Operations */
#define RESET_ENABLE_CMD                     	0x66
#define RESET_MEMORY_CMD                     	0x99

/*QPI Operations */
#define ENTER_QPI_MODE_CMD                   	0x38
#define EXIT_QPI_MODE_CMD                    	0xFF
	
/* Identification Operations */	
#define READ_ID_CMD                          	0x90
#define DUAL_READ_ID_CMD                     	0x92
#define QUAD_READ_ID_CMD                     	0x94
#define READ_JEDEC_ID_CMD                    	0x9F
	
/* Read Operations */	
#define READ_CMD                             	0x03
#define FAST_READ_CMD                        	0x0B
#define DUAL_OUT_FAST_READ_CMD               	0x3B
#define DUAL_INOUT_FAST_READ_CMD             	0xBB
#define QUAD_OUT_FAST_READ_CMD               	0x6B
#define QUAD_INOUT_FAST_READ_CMD             	0xEB
	
/* Write Operations */	
#define WRITE_ENABLE_CMD                     	0x06
#define WRITE_DISABLE_CMD                    	0x04

/* Read Status Register Operations */
#define READ_STATUS_REG1_CMD                  0x05
#define READ_STATUS_REG2_CMD                  0x35
#define READ_STATUS_REG3_CMD                  0x15

/* Write Status Register Operations */
#define WRITE_STATUS_REG1_CMD                 0x01
#define WRITE_STATUS_REG2_CMD                 0x31
#define WRITE_STATUS_REG3_CMD                 0x11

/* Program Operations */
#define PAGE_PROG_CMD                        	0x02
#define QUAD_INPUT_PAGE_PROG_CMD             	0x32
	
/* Erase Operations */	
#define SECTOR_ERASE4KB_CMD                   0x20 //Sector Erase (4KB)
#define BLOCK_ERASE32KB_CMD									 	0x52 //Block Erase (32KB)
#define BLOCK_ERASE64KB_CMD									 	0xD8 //Block Erase (64KB)
#define CHIP_ERASE_CMD                       	0x60 //Chip Erase

/* Program/Erase Operations */
#define PROG_ERASE_RESUME_CMD                	0x7A
#define PROG_ERASE_SUSPEND_CMD               	0x75

/* 4-byte Address Mode Operations */
#define ENTER_4BTYE_ADDR_MODE_CMD           	0xB7
#define EXIT_4BTYE_ADDR_MODE_CMD            	0xE9

/* Flag Status Register */
#define W25QxxFV_FSR_BUSY                    	((uint8_t)0x01)    /*!< busy */
#define W25QxxFV_FSR_WREN                    	((uint8_t)0x02)    /*!< write enable */
#define W25QxxFV_FSR_QE                      	((uint8_t)0x02)    /*!< quad enable */



#define W25Qx_OK            									((uint8_t)0x00)
#define W25Qx_ERROR         									((uint8_t)0x01)
#define W25Qx_BUSY          									((uint8_t)0x02)
#define W25Qx_TIMEOUT													((uint8_t)0x03)


#define W25QxxFV_TIMEOUT_VALUE			      1000	
#define W25QxxFV_ERASE_CHIP_TIMEOUT_VALUE	30000


#define W25Qx_CS_Enable() 										HAL_GPIO_WritePin(SPI5_NSS_GPIO_Port, SPI5_NSS_Pin, GPIO_PIN_RESET)
#define W25Qx_CS_Disable() 										HAL_GPIO_WritePin(SPI5_NSS_GPIO_Port, SPI5_NSS_Pin, GPIO_PIN_SET)


uint8_t 	BSP_W25Qx_Init(void);
uint8_t	  BSP_W25Qx_Reset(void);
uint8_t   BSP_W25Qx_GetStatus(uint8_t	cmd);
uint8_t   BSP_W25Qx_Write_Function(uint8_t cmd);
uint16_t	BSP_W25Qx_Read_ID(void);
uint8_t   BSP_W25Qx_4ByteAddressMode_Function(uint8_t cmd);

uint8_t 	BSP_W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size);
uint8_t 	BSP_W25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
	
uint8_t 	BSP_W25Qx_Erase_OneSector(uint32_t Address);
uint8_t 	BSP_W25Qx_Erase_MultiSector(uint32_t Address, uint32_t Size);
uint8_t 	BSP_W25Qx_Erase_Chip(void);

#endif




