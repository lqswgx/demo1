/*
 * @Author: LQS
 * @Date: 2025-04-28 21:46:56
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 15:09:18
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Core\Src\flash.c
 * @Description: 
 */
#include "flash.h"

DeviceInfoTypeDef device_info;
FLASH_HandleTypeDef flash_handle;
/* Timeout value in milliseconds */
#define FLASH_TIMEOUT_VALUE    50000U
#define FLASH_DEFAULT_RETRY    3U

/* Sector address map for STM32F429IGT6 */
static const uint32_t sector_base_addresses[] = {
    0x08000000U, // Sector 0: 16KB
    0x08004000U, // Sector 1: 16KB
    0x08008000U, // Sector 2: 16KB
    0x0800C000U, // Sector 3: 16KB
    0x08010000U, // Sector 4: 64KB
    0x08020000U, // Sector 5: 128KB
    0x08040000U, // Sector 6: 128KB
    0x08060000U, // Sector 7: 128KB
    0x08080000U, // Sector 8: 128KB
    0x080A0000U, // Sector 9: 128KB
    0x080C0000U, // Sector 10: 128KB
    0x080E0000U, // Sector 11: 128KB
    0x08100000U, // Sector 12: 16KB
    0x08104000U, // Sector 13: 16KB
    0x08108000U, // Sector 14: 16KB
    0x0810C000U, // Sector 15: 16KB
    0x08110000U, // Sector 16: 64KB
    0x08120000U, // Sector 17: 128KB
    0x08140000U, // Sector 18: 128KB
    0x08160000U, // Sector 19: 128KB
    0x08180000U, // Sector 20: 128KB
    0x081A0000U, // Sector 21: 128KB
    0x081C0000U, // Sector 22: 128KB
    0x081E0000U  // Sector 23: 128KB
};

/**
 * @brief  Get sector number for a given address
 * @param  address: Flash address
 * @retval Sector number or -1 if invalid
 */
static int32_t FLASH_GetSectorNumber(uint32_t address)
{
    if (address < FLASH_BASE_ADDR || address >= (FLASH_BASE_ADDR + FLASH_TOTAL_SIZE))
    {
        return -1;
    }
    
    for (uint32_t i = 0; i < 24; i++)
    {
        if (address >= sector_base_addresses[i] && 
            (i == 23 || address < sector_base_addresses[i + 1]))
        {
            return i;
        }
    }
    return -1;
}

/**
 * @brief  Calculate checksum for device info
 * @param  info: Pointer to device info structure
 * @retval Checksum value
 */
static uint16_t FLASH_CalculateChecksum(const DeviceInfoTypeDef *info)
{
    uint16_t checksum = 0;
    for (uint32_t i = 0; i < DEVICE_DATA_SIZE; i++)
    {
        checksum += info->data[i];
    }
    checksum += info->constant;
    return checksum;
}

/**
 * @brief  Initialize Flash interface
 * @param  handle: Flash driver handle
 * @param  info: Pointer to device info structure
 * @retval FLASH_Status
 */
FLASH_Status FLASH_Init(FLASH_HandleTypeDef *handle, DeviceInfoTypeDef *info)
{
    FLASH_Status status;

    if (handle == NULL)
    {
        LOG_FLASH("FLASH_Init: Invalid handle\n");
        return FLASH_INVALID_PARAM;
    }

    /* Create mutex for FreeRTOS */
    handle->mutex = xSemaphoreCreateMutex();
    if (handle->mutex == NULL)
    {
        return FLASH_ERROR;
    }
    
    handle->retry_count = FLASH_DEFAULT_RETRY;
    
    /* Unlock Flash control register */
    FLASH->KEYR = 0x45670123U;
    FLASH->KEYR = 0xCDEF89ABU;
    
    /* Clear pending flags */
    FLASH->SR = (FLASH_SR_PGSERR | FLASH_SR_PGPERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR);

    //Read device info from flash
    status = FLASH_VerifyDeviceInfo(handle, info);
    if(status != FLASH_OK)
    {
        if(status == FLASH_CHECKSUM_ERROR)
        {
            LOG_FLASH("FLASH_Init: Device info checksum error\n");
            //store default device info
            if(FLASH_StoreDeviceInfo(handle, info) != FLASH_OK)
            {
                LOG_FLASH("FLASH_Init: Failed to store default device info\n");
                return status;
            }
        }
        else
        {
            LOG_FLASH("FLASH_Init: Failed to read device info, status = %d\n", status);
            return status;
        }
    }

    LOG_FLASH("FLASH_Init: Initialized\n");
    return FLASH_OK;
}

/**
 * @brief  Wait for Flash operation to complete
 * @param  handle: Flash driver handle
 * @param  timeout: Timeout duration in milliseconds
 * @retval FLASH_Status
 */
FLASH_Status FLASH_WAITForLastOperation(FLASH_HandleTypeDef *handle, uint32_t timeout)
{
    uint32_t tickstart = xTaskGetTickCount();
    
    /* Wait for Flash busy flag to be cleared */
    while (FLASH->SR & FLASH_SR_BSY)
    {
        if ((xTaskGetTickCount() - tickstart) > pdMS_TO_TICKS(timeout))
        {
            return FLASH_TIMEOUT;
        }
        taskYIELD(); // Yield to other tasks
    }
    
    /* Check for errors */
    if (FLASH->SR & (FLASH_SR_PGSERR | FLASH_SR_PGPERR | FLASH_SR_PGAERR | FLASH_SR_WRPERR))
    {
        LOG_FLASH("FLASH_WAITForLastOperation: Error\n");
        return FLASH_ERROR;
    }
    
    return FLASH_OK;
}

/**
 * @brief  Erase specified Flash sector
 * @param  handle: Flash driver handle
 * @param  sector: Sector number to erase (0 to 23 for STM32F429IGT6)
 * @retval FLASH_Status
 */
FLASH_Status FLASH_EraseSector(FLASH_HandleTypeDef *handle, uint32_t sector)
{
    FLASH_Status status = FLASH_OK;
    uint32_t retry = handle->retry_count;
    
    if (handle == NULL || handle->mutex == NULL)
    {
        LOG_FLASH("FLASH_EraseSector: Invalid handle\n");
        return FLASH_INVALID_PARAM;
    }
    
    /* Check sector validity */
    if (sector > 23)
    {
        LOG_FLASH("FLASH_EraseSector: Invalid sector number\n");
        return FLASH_INVALID_ADDR;
    }
    
    do {
        /* Wait for last operation */
        status = FLASH_WAITForLastOperation(handle, FLASH_TIMEOUT_VALUE);
        if (status != FLASH_OK)
        {
            continue;
        }
        
        /* Set sector erase parameters */
        FLASH->CR &= ~(FLASH_CR_PSIZE | FLASH_CR_SNB);
        FLASH->CR |= (FLASH_CR_SER | (sector << FLASH_CR_SNB_Pos) | FLASH_PSIZE_WORD);
        
        /* Start erase operation */
        FLASH->CR |= FLASH_CR_STRT;
        
        /* Wait for operation to complete */
        status = FLASH_WAITForLastOperation(handle, FLASH_TIMEOUT_VALUE);
        
        /* Disable sector erase */
        FLASH->CR &= (~FLASH_CR_SER);
        
    } while (status != FLASH_OK && --retry > 0);
    
    return status;
}

/**
 * @brief  Erase all sectors covering a given address range
 * @param  handle: Flash driver handle
 * @param  start_addr: Start address of the range
 * @param  length: Length of the data
 * @retval FLASH_Status
 */
FLASH_Status FLASH_EraseSectorsForRange(FLASH_HandleTypeDef *handle, uint32_t start_addr, uint32_t length)
{
    if (handle == NULL || handle->mutex == NULL)
    {
        LOG_FLASH("FLASH_EraseSectorsForRange: Invalid handle\n");
        return FLASH_INVALID_PARAM;
    }
    
    /* Check address validity */
    if ((start_addr < FLASH_BASE_ADDR) || ((start_addr + length) > (FLASH_BASE_ADDR + FLASH_TOTAL_SIZE)))
    {
        LOG_FLASH("FLASH_EraseSectorsForRange: Invalid address range\n");
        return FLASH_INVALID_ADDR;
    }
    
    /* Take mutex */
    if (xSemaphoreTake(handle->mutex, pdMS_TO_TICKS(FLASH_TIMEOUT_VALUE)) != pdTRUE)
    {
        LOG_FLASH("FLASH_EraseSectorsForRange: Failed to take mutex\n");
        return FLASH_LOCK_ERROR;
    }
    
    /* Find start and end sectors */
    int32_t start_sector = FLASH_GetSectorNumber(start_addr);
    int32_t end_sector = FLASH_GetSectorNumber(start_addr + length - 1);
    
    if (start_sector == -1 || end_sector == -1)
    {
        xSemaphoreGive(handle->mutex);
        LOG_FLASH("FLASH_EraseSectorsForRange: Invalid sector number\n");
        return FLASH_INVALID_ADDR;
    }
    
    /* Erase all sectors in range */
    FLASH_Status status = FLASH_OK;
    for (int32_t sector = start_sector; sector <= end_sector; sector++)
    {
        status = FLASH_EraseSector(handle, sector);
        if (status != FLASH_OK)
        {
            xSemaphoreGive(handle->mutex);
            LOG_FLASH("FLASH_EraseSectorsForRange: Sector %d erase failed, status = %d\n", sector, status);
            return status;
        }
        LOG_FLASH("FLASH_EraseSectorsForRange: Sector %d erased\n", sector);
    }
    
    xSemaphoreGive(handle->mutex);
    return FLASH_OK;
}

/**
 * @brief  Write data to Flash
 * @param  handle: Flash driver handle
 * @param  address: Destination address
 * @param  data: Data to write
 * @param  length: Length of data in bytes
 * @retval FLASH_Status
 */
FLASH_Status FLASH_WriteData(FLASH_HandleTypeDef *handle, uint32_t address, const uint8_t *data, uint32_t length)
{
    FLASH_Status status = FLASH_OK;
    uint32_t retry = handle->retry_count;
    uint32_t i, aligned_length, remaining_bytes;
    uint32_t word_data;
    
    if (handle == NULL || handle->mutex == NULL || data == NULL || length == 0)
    {
        LOG_FLASH("FLASH_WriteData: Invalid parameter\n");
        return FLASH_INVALID_PARAM;
    }
    
    /* Check address validity */
    if ((address < FLASH_BASE_ADDR) || ((address + length) > (FLASH_BASE_ADDR + FLASH_TOTAL_SIZE)))
    {
        LOG_FLASH("FLASH_WriteData: Invalid address range\n");
        return FLASH_INVALID_ADDR;
    }
    
    /* Take mutex */
    if (xSemaphoreTake(handle->mutex, pdMS_TO_TICKS(FLASH_TIMEOUT_VALUE)) != pdTRUE)
    {
        LOG_FLASH("FLASH_WriteData: Failed to take mutex\n");
        return FLASH_LOCK_ERROR;
    }
    
    do {
        /* Wait for last operation */
        status = FLASH_WAITForLastOperation(handle, FLASH_TIMEOUT_VALUE);
        if (status != FLASH_OK)
        {
            continue;
        }
        
        /* Enable programming */
        FLASH->CR &= ~(FLASH_CR_PSIZE);
        FLASH->CR |= (FLASH_CR_PG | FLASH_PSIZE_WORD);
        
        /* Write word-aligned data */
        aligned_length = length & ~0x3U; // Align to 4 bytes
        for (i = 0; i < aligned_length; i += 4)
        {
            word_data = (data[i] | (data[i+1] << 8) | (data[i+2] << 16) | (data[i+3] << 24));
            *(__IO uint32_t*)(address + i) = word_data;
            
            status = FLASH_WAITForLastOperation(handle, FLASH_TIMEOUT_VALUE);
            if (status != FLASH_OK)
            {
                LOG_FLASH("FALSH_WriteData: Wait for last operation timeout\n");
                LOG_FLASH("FLASH_WriteData: Write error at address 0x%08X\n", address + i);
                break;
            }
        }
        
        /* Write remaining bytes */
        remaining_bytes = length & 0x3U;
        if (status == FLASH_OK && remaining_bytes > 0)
        {
            word_data = 0xFFFFFFFFU; // Fill with 0xFF
            for (uint32_t j = 0; j < remaining_bytes; j++)
            {
                word_data &= ~(0xFFU << (j * 8));
                word_data |= (data[i + j] << (j * 8));
            }
            *(__IO uint32_t*)(address + i) = word_data;
            
            status = FLASH_WAITForLastOperation(handle, FLASH_TIMEOUT_VALUE);
        }
        
        /* Disable programming */
        FLASH->CR &= (~FLASH_CR_PG);
        
    } while (status != FLASH_OK && --retry > 0);
    
    xSemaphoreGive(handle->mutex);
    return status;
}

/**
 * @brief  Read data from Flash
 * @param  handle: Flash driver handle
 * @param  address: Source address
 * @param  data: Buffer to store read data
 * @param  length: Length of data in bytes
 * @retval FLASH_Status
 */
FLASH_Status FLASH_ReadData(FLASH_HandleTypeDef *handle, uint32_t address, uint8_t *data, uint32_t length)
{
    uint32_t i, word_data;
    
    if (handle == NULL || data == NULL || length == 0)
    {
        LOG_FLASH("FLASH_ReadData: Invalid parameter\n");
        return FLASH_INVALID_PARAM;
    }
    
    /* Check address validity */
    if ((address < FLASH_BASE_ADDR) || ((address + length) > (FLASH_BASE_ADDR + FLASH_TOTAL_SIZE)))
    {
        LOG_FLASH("FLASH_ReadData: Invalid address range\n");
        return FLASH_INVALID_ADDR;
    }
    
    /* Read data byte by byte */
    for (i = 0; i < length; i += 4)
    {
        word_data = *(__IO uint32_t*)(address + i);
        
        if (i + 0 < length) data[i + 0] = (word_data >> 0) & 0xFF;
        if (i + 1 < length) data[i + 1] = (word_data >> 8) & 0xFF;
        if (i + 2 < length) data[i + 2] = (word_data >> 16) & 0xFF;
        if (i + 3 < length) data[i + 3] = (word_data >> 24) & 0xFF;
    }
    
    return FLASH_OK;
}

/**
 * @brief  Store device information to Flash
 * @param  handle: Flash driver handle
 * @param  info: Pointer to device info structure
 * @retval FLASH_Status
 */
FLASH_Status FLASH_StoreDeviceInfo(FLASH_HandleTypeDef *handle, DeviceInfoTypeDef *info)
{
    FLASH_Status status;
    
    if (handle == NULL || info == NULL)
    {
        LOG_FLASH("FLASH_StoreDeviceInfo: Invalid parameter\n");
        return FLASH_INVALID_PARAM;
    }

    /* Store constant value */
    info ->constant = DEVICE_INFO_CONST;

    /* Calculate checksum */
    info->checksum = FLASH_CalculateChecksum(info);
    
    /* Erase sectors covering the device info range */
    status = FLASH_EraseSectorsForRange(handle, DEVICE_INFO_ADDR, sizeof(DeviceInfoTypeDef));
    if (status != FLASH_OK)
    {
        LOG_FLASH("FLASH_StoreDeviceInfo: Erase failed, status = %d\n", status);
        return status;
    }
    
    /* Write device info structure */
    status = FLASH_WriteData(handle, DEVICE_INFO_ADDR, (uint8_t *)info, sizeof(DeviceInfoTypeDef));
    if (status != FLASH_OK)
    {
        LOG_FLASH("FLASH_StoreDeviceInfo: Write failed, status = %d\n", status);
        return status;
    }
    
    /* Verify by reading back and checking checksum */
    DeviceInfoTypeDef temp_info;
    status = FLASH_ReadData(handle, DEVICE_INFO_ADDR, (uint8_t *)&temp_info, sizeof(DeviceInfoTypeDef));
    if (status != FLASH_OK)
    {
        LOG_FLASH("FLASH_StoreDeviceInfo: Read failed, status = %d\n", status);
        return status;
    }
    
    if (temp_info.checksum != FLASH_CalculateChecksum(&temp_info))
    {
        LOG_FLASH("FLASH_StoreDeviceInfo: Checksum error\n");
        return FLASH_CHECKSUM_ERROR;
    }
    
    LOG_FLASH("FLASH_StoreDeviceInfo: Device info stored\n");
    return FLASH_OK;
}

/**
 * @brief  Read and verify device information from Flash
 * @param  handle: Flash driver handle
 * @param  info: Pointer to store device info structure
 * @retval FLASH_Status
 */
FLASH_Status FLASH_VerifyDeviceInfo(FLASH_HandleTypeDef *handle, DeviceInfoTypeDef *info)
{
    FLASH_Status status;
    
    if (handle == NULL || info == NULL)
    {
        LOG_FLASH("FLASH_VerifyDeviceInfo: Invalid parameter\n");
        return FLASH_INVALID_PARAM;
    }
    
    /* Read device info structure */
    status = FLASH_ReadData(handle, DEVICE_INFO_ADDR, (uint8_t *)info, sizeof(DeviceInfoTypeDef));
    if (status != FLASH_OK)
    {
        LOG_FLASH("FLASH_VerifyDeviceInfo: Read failed, status = %d\n", status);
        return status;
    }
    
    /* Verify checksum */
    if (info->checksum != FLASH_CalculateChecksum(info))
    {
        LOG_FLASH("FLASH_VerifyDeviceInfo: Checksum error\n");
        return FLASH_CHECKSUM_ERROR;
    }
    
    LOG_FLASH("FLASH_VerifyDeviceInfo: Device info verified\n");
    return FLASH_OK;
}

