/*
 * @Author: LQS
 * @Date: 2025-04-28 21:47:07
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 16:33:59
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Core\Inc\flash.h
 * @Description: 
 */
#ifndef __FLASH_H_
#define __FLASH_H_
#include "main.h"
#include "us_freertos.h"

/* Flash configuration */
/*
 * STM32F42xxx and STM32F43xxx Flash size:2M
 * FLASH_BASE_ADDR: 0x08000000
 * FLASH_END_ADDR: 0x081FFFFF
 * BANK1: 0x08000000 - 0x080FFFFF
 * SECTOR0: 0x08000000 - 0x08003FFF(16KB)
 * SECTOR1: 0x08004000 - 0x08007FFF(16KB)
 * SECTOR2: 0x08008000 - 0x0800BFFF(16KB)
 * SECTOR3: 0x0800C000 - 0x0800FFFF(16KB)
 * SECTOR4: 0x08010000 - 0x0801FFFF(64KB)
 * SECTOR5: 0x08020000 - 0x0803FFFF(128KB)
 * SECTOR6: 0x08040000 - 0x0805FFFF(128KB)
 * ......
 * SECTOR11: 0x080E0000 - 0x080FFFFF(128KB)
 * BANK2: 0x08100000 - 0x081FFFFF
 * SECTOR12: 0x08100000 - 0x08103FFF(16KB)
 * SECTOR13: 0x08104000 - 0x08107FFF(16KB)
 * SECTOR14: 0x08108000 - 0x0810BFFF(16KB)
 * SECTOR15: 0x0810C000 - 0x0810FFFF(16KB)
 * SECTOR16: 0x08110000 - 0x0811FFFF(64KB)
 * SECTOR17: 0x08120000 - 0x0813FFFF(128KB)
 * SECTOR18: 0x08140000 - 0x0815FFFF(128KB)
 * ......
 * SECTOR23: 0x081E0000 - 0x081FFFFF(128KB)
 */

#define FLASH_BASE_ADDR        0x08000000U  // FLASH base address
#define FLASH_TOTAL_SIZE       0x200000U    // 2MB for STM32F429IGT6

#define DEVICE_INFO_CONST      0x55U        // Constant value for device info structure
#define DEVICE_INFO_ADDR       0x08040000U  // Store device info in sector 6

/* Flash operation status */
typedef enum
{
    FLASH_OK = 0,
    FLASH_ERROR,
    FLASH_BUSY,
    FLASH_TIMEOUT,
    FLASH_INVALID_ADDR,
    FLASH_INVALID_PARAM,
    FLASH_LOCK_ERROR,
    FLASH_CHECKSUM_ERROR
} FLASH_Status;

/* Device information structure */
#define DEVICE_DATA_SIZE       512U // Size of data array in bytes
typedef struct
{
    uint8_t data[DEVICE_DATA_SIZE]; // Data array
    uint8_t major;                  // Major version number
    uint8_t minor;                  // Minor version number
    uint8_t patch;                  // Patch version number
    uint8_t reserved[20];           // Reserved for future use
    uint8_t constant;               // Constant value
    uint16_t checksum;              // Checksum for validation
} DeviceInfoTypeDef;
extern DeviceInfoTypeDef device_info;

/* Flash driver handle */
typedef struct
{
    SemaphoreHandle_t mutex; // FreeRTOS mutex for thread safety
    uint32_t retry_count;    // Max retry attempts for operations
} FLASH_HandleTypeDef;
extern FLASH_HandleTypeDef flash_handle;

/* Function prototypes */
FLASH_Status FLASH_Init(FLASH_HandleTypeDef *handle, DeviceInfoTypeDef *info);
FLASH_Status FLASH_EraseSector(FLASH_HandleTypeDef *handle, uint32_t sector);
FLASH_Status FLASH_EraseSectorsForRange(FLASH_HandleTypeDef *handle, uint32_t start_addr, uint32_t length);
FLASH_Status FLASH_WriteData(FLASH_HandleTypeDef *handle, uint32_t address, const uint8_t *data, uint32_t length);
FLASH_Status FLASH_ReadData(FLASH_HandleTypeDef *handle, uint32_t address, uint8_t *data, uint32_t length);
FLASH_Status FLASH_WAITForLastOperation(FLASH_HandleTypeDef *handle, uint32_t timeout);
FLASH_Status FLASH_StoreDeviceInfo(FLASH_HandleTypeDef *handle, DeviceInfoTypeDef *info);
FLASH_Status FLASH_VerifyDeviceInfo(FLASH_HandleTypeDef *handle, DeviceInfoTypeDef *info);

#endif



 
