/**
  * @file    w25qxx.c
  * @brief   This file provides a set of functions needed to manage the W25QxxFV QSPI memory.
  * @version V1.0.0
  * @date    2025-03-27
  * @author  LQS
  * @note    This file is part of the standard driver library for the W25QxxFV QSPI memory.
  *
  */
#include "w25qxx.h"

uint16_t W25Qxx_ID=W25Q256;

//W25Q256FV Flash Configuration
#define W25QxxFV_STARTADDR				0x00000000 	//定义Flash的起始地址
#define W25QxxFV_ENDADDR				0x01FFFFFF 	//定义Flash的结束地址
#define W25QxxFV_FLASH_SIZE 			0x10000000 	//定义Flash的总大小
#define W25QxxFV_BLOCK_SIZE 			0x10000    	//定义Flash的块大小
#define W25QxxFV_SECTOR_SIZE			0x1000     	//定义Flash的扇区大小
#define W25QxxFV_PAGE_SIZE  			0x100      	//定义Flash的页大小
#define W25QxxFV_BLOCK_NUMBER			0x200      	//定义Flash的块数量
#define W25QxxFV_SECTOR_NUMBER   		0x2000     	//定义Flash的扇区数量
#define W25QxxFV_SECTOR_PER_BLOCK		0x10       	//定义每个块中的扇区数量
#define W25QxxFV_PAGE_PER_SECTOR 		0x10       	//定义每个扇区中的页数量
#define W25QxxFV_PAGE_PER_BLOCK  		0x100     	//定义每个块中的页数量

/**
 * @brief  Initializes the W25QxxFV.
 * @retval W25QxxFV memory status
 */
uint8_t BSP_W25Qx_Init(void)
{ 
	uint8_t status=0;
	
	//Reset the W25Qxxx
	BSP_W25Qx_Reset();

	//Read the W25Qxxx ID
	W25Qxx_ID = BSP_W25Qx_Read_ID();
	//Check the W25Qxxx ID
	if(W25Qxx_ID >= W25Q256)
	{
		//Enter 4-byte address mode
		status=BSP_W25Qx_4ByteAddressMode_Function(ENTER_4BTYE_ADDR_MODE_CMD);
		if(status!=W25Qx_OK)
		{
			LOG_W25QXX("BSP_W25Qx_Init: W25QxxFV 4ByteAddressMode Error!\r\n");
		}
		else
		{
			LOG_W25QXX("BSP_W25Qx_Init: W25QxxFV 4ByteAddressMode OK!\r\n");
		}
	}
	else
	{
		LOG_W25QXX("BSP_W25Qx_Init: W25QxxFV ID Error!\r\n");
		status = W25Qx_ERROR;
	}

	return status;
}

/**
  * @brief  This function reset the W25Qx.
	* @retval W25QxxFV memory status
  */
uint8_t	BSP_W25Qx_Reset(void)
{
	uint8_t status=0;
	TickType_t xStartTime = xTaskGetTickCount();
	TickType_t xElapsedTime = 0;
	BaseType_t xBusy = pdTRUE;	

	uint8_t cmd[2] = {RESET_ENABLE_CMD,RESET_MEMORY_CMD};
	
	W25Qx_CS_Enable();
	
	/* Send the reset command */
	SPI_ReadWriteByte(cmd[0]);	
	SPI_ReadWriteByte(cmd[1]);		
	
	W25Qx_CS_Disable();
	
	/*wait for the W25Qxxx ready*/
	while(xBusy && (xElapsedTime < W25QxxFV_TIMEOUT_VALUE))
	{
		status = BSP_W25Qx_GetStatus(READ_STATUS_REG1_CMD);
		if(status != W25Qx_BUSY)
		{
			xBusy = pdFALSE;
			break;
		}
		xElapsedTime++;
		delay_ms(1);
	}

	// while (xBusy && (xElapsedTime < W25QxxFV_TIMEOUT_VALUE))
	// {
	// 	status = BSP_W25Qx_GetStatus(READ_STATUS_REG1_CMD);
	// 	if(status != W25Qx_BUSY)
	// 	{
	// 		xBusy = pdFALSE;
	// 	}
	// 	xElapsedTime = xTaskGetTickCount() - xStartTime;
	// }
	/* Check for timeout */
	if (xElapsedTime >= W25QxxFV_TIMEOUT_VALUE) 
	{
		LOG_W25QXX("BSP_W25Qx_Reset: W25QxxFV wait TimeOut!\r\n");
	 	return	W25Qx_TIMEOUT;
	}
	/* Check the value of the register */
	if(status!= W25Qx_OK)
	{
		return W25Qx_ERROR;
	}

	return W25Qx_OK;
}

/**
 * @brief  Enter or Exit the 4-byte address mode.
 * @param  cmd: 4-byte address mode command.
 * @retval W25QxxFV memory status
 */
uint8_t BSP_W25Qx_4ByteAddressMode_Function(uint8_t cmd)
{
	uint8_t status=0;
	TickType_t xStartTime = xTaskGetTickCount();
	TickType_t xElapsedTime = 0;
	BaseType_t xBusy = pdTRUE;	
	
	/* Check the command */
	if (cmd==ENTER_4BTYE_ADDR_MODE_CMD || cmd==EXIT_4BTYE_ADDR_MODE_CMD)
	{
		W25Qx_CS_Enable();
		/* Send the 4-byte address mode command */
		SPI_ReadWriteByte(cmd);		
		W25Qx_CS_Disable();
	}
	else
	{
		LOG_W25QXX("BSP_W25Qx_4ByteAddressMode_Function: Invalid command!\r\n");
		return W25Qx_ERROR;
	}

	/* Wait for W25Qxxx ready */
	while(xBusy && (xElapsedTime < W25QxxFV_TIMEOUT_VALUE))
	{
		status = BSP_W25Qx_GetStatus(READ_STATUS_REG1_CMD);
		if(status != W25Qx_BUSY)
		{
			xBusy = pdFALSE;
			break;
		}
		xElapsedTime++;
		delay_ms(1);
	}
	/* Check for timeout */
	if (xElapsedTime >= W25QxxFV_TIMEOUT_VALUE) 
	{
		LOG_W25QXX("BSP_W25Qx_4ByteAddressMode_Function: W25QxxFV wait TimeOut!\r\n");
		return	W25Qx_TIMEOUT;
	}
	if (status!= W25Qx_OK)
	{
		return W25Qx_ERROR;
	}	

	return W25Qx_OK;
}

/**
  * @brief  Reads current status of the W25QxxFV.
  * @param  cmd: Status register to read.
  * @retval W25QxxFV memory status
  */
uint8_t BSP_W25Qx_GetStatus(uint8_t	cmd)
{
	uint8_t status=0;
	
	//* Check the command */
	if(cmd==READ_STATUS_REG1_CMD || cmd==READ_STATUS_REG2_CMD || cmd==READ_STATUS_REG3_CMD)
	{
		W25Qx_CS_Enable();
		/* Send the read status command */
		SPI_ReadWriteByte(cmd);		
		/* Reception of the data */
		status=SPI_ReadWriteByte(0xff);	
		W25Qx_CS_Disable();
	}
	else
	{
		LOG_W25QXX("BSP_W25Qx_GetStatus: BSP_W25Qx_GetStatus:Invalid command!\r\n");
		return W25Qx_ERROR;
	}

	/* Check the value of the register */
	if((status & W25QxxFV_FSR_BUSY) != 0)
	{
		return W25Qx_BUSY;
	}
	else
	{
		return W25Qx_OK;
	}		
}

/**
 * @brief  Enables or disables the write operations.
 * @param  cmd: Write operation command.
 * @retval W25QxxFV memory status
 */
uint8_t BSP_W25Qx_Write_Function(uint8_t cmd)
{
	uint8_t status=0;
    TickType_t xStartTime = xTaskGetTickCount();
    TickType_t xElapsedTime = 0;
    BaseType_t xBusy = pdTRUE;

	/* Check the command */
	if (cmd==WRITE_ENABLE_CMD || cmd==WRITE_DISABLE_CMD)
	{
		/*Select the FLASH: Chip Select low */
		W25Qx_CS_Enable();
		/* Send the read ID command */
		SPI_ReadWriteByte(cmd);
		/*Deselect the FLASH: Chip Select high */
		W25Qx_CS_Disable();
	}
	else
	{
		LOG_W25QXX("BSP_W25Qx_Write_Function:Invalid command!\r\n");
		return W25Qx_ERROR;
	}

	/* Wait the end of Flash writing */
	while(xBusy && (xElapsedTime < W25QxxFV_TIMEOUT_VALUE))
	{
		status = BSP_W25Qx_GetStatus(READ_STATUS_REG1_CMD);
		if(status != W25Qx_BUSY)
		{
			xBusy = pdFALSE;
			break;
		}
		xElapsedTime++;
		delay_ms(1);
	}

	/* Check for timeout */
	if (xElapsedTime >= W25QxxFV_TIMEOUT_VALUE) 
	{
		LOG_W25QXX("BSP_W25Qx_Write_Function: W25QxxFV wait TimeOut!\r\n");
    	return	W25Qx_TIMEOUT;
	}
	/* Check the value of the register */
	if(status != W25Qx_OK)
	{
		return W25Qx_ERROR;
	}
	
	return W25Qx_OK;
}

 /**
  * @brief  Reads the Manufacture/Device ID.
  * @retval The Manufacture/Device ID.
  * @note   The Manufacture/Device ID is a 16-bit unique identifier.
  */
uint16_t BSP_W25Qx_Read_ID(void)
{
	uint8_t cmd[4] = {READ_ID_CMD,0x00,0x00,0x00};
	uint16_t ID=0;
	
	W25Qx_CS_Enable();
	/* Send the read ID command */
	SPI_ReadWriteByte(cmd[0]);
	SPI_ReadWriteByte(cmd[1]);
	SPI_ReadWriteByte(cmd[2]);
	SPI_ReadWriteByte(cmd[3]);	
	/* Reception of the data */
	ID|=SPI_ReadWriteByte(0xff)<<8;
	ID|=SPI_ReadWriteByte(0xff);
	W25Qx_CS_Disable();
	
	return ID;
}

/**
  * @brief  Reads an amount of data from the QSPI memory.
  * @param  pData: Pointer to data to be stored
  * @param  ReadAddr: Read start address
  * @param  Size: Size of data to read    
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Read(uint8_t* pData, uint32_t ReadAddr, uint32_t Size)
{
	W25Qx_CS_Enable();

	/* Send the read ID command */
	SPI_ReadWriteByte(READ_CMD);
	if(W25Qxx_ID >= W25Q256)
	{
		SPI_ReadWriteByte((uint8_t)(ReadAddr >> 24));
	}
	SPI_ReadWriteByte((uint8_t)(ReadAddr >> 16));
	SPI_ReadWriteByte((uint8_t)(ReadAddr >> 8));
	SPI_ReadWriteByte((uint8_t)(ReadAddr));		
	/* Reception of the data */
	for(uint8_t i=0;i<Size;i++)
	{
		pData[i]=SPI_ReadWriteByte(0xff);
	}
	W25Qx_CS_Disable();
	
	return W25Qx_OK;
}

/**
  * @brief  Writes an amount of data to the QSPI memory.
  * @param  pData: Pointer to data to be written
  * @param  WriteAddr: Write start address
  * @param  Size: Size of data to write   
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size)
{
	uint8_t status=0;
	TickType_t xStartTime = xTaskGetTickCount();
	TickType_t xElapsedTime = 0;
	BaseType_t xBusy = pdTRUE;

	uint32_t end_addr=0, current_size=0, current_addr=0;
	
	/* Calculation of the size between the write address and the end of the page */
	do
	{
		current_addr += W25QxxFV_PAGE_SIZE;
	} while (current_addr <= WriteAddr);
  	current_size = current_addr - WriteAddr;

  	/* Check if the size of the data is less than the remaining place in the page */
	if (current_size > Size)
	{
		current_size = Size;
	}

	/* Initialize the adress variables */
	current_addr = WriteAddr;
	end_addr = WriteAddr + Size;

	/*锟斤拷锟斤拷写锟斤拷锟斤拷锟捷的筹拷锟饺ｏ拷锟斤拷锟斤拷锟斤拷应锟斤拷锟斤拷锟斤拷*/
	status = BSP_W25Qx_Erase_MultiSector(WriteAddr,Size);
	if(status!=W25Qx_OK)
	{
		LOG_W25QXX("BSP_W25Qx_Write: BSP_W25Qx_Erase_MultiSector Error!\r\n");
		return W25Qx_ERROR;
	}

  /* Perform the write page by page */
  do
  {
		/* Enable write operations */
		BSP_W25Qx_Write_Function(WRITE_ENABLE_CMD);
	
		W25Qx_CS_Enable();
    /* Send the command */
		SPI_ReadWriteByte(PAGE_PROG_CMD);
		if (W25Qxx_ID >= W25Q256)
		{
			SPI_ReadWriteByte((uint8_t)(current_addr >> 24));
		}
		SPI_ReadWriteByte((uint8_t)(current_addr >> 16));
		SPI_ReadWriteByte((uint8_t)(current_addr >> 8));
		SPI_ReadWriteByte((uint8_t)(current_addr));			
    
    /* Transmission of the data */
		for(uint8_t i=0;i<current_size;i++)
		{
			SPI_ReadWriteByte(pData[i]);
		}
		W25Qx_CS_Disable();
  
		/*wait for the W25Qxxx ready*/
		while(xBusy && (xElapsedTime < W25QxxFV_TIMEOUT_VALUE))
		{
			status = BSP_W25Qx_GetStatus(READ_STATUS_REG1_CMD);
			if(status != W25Qx_BUSY)
			{
				xBusy = pdFALSE;
				break;
			}
			xElapsedTime++;
			delay_ms(1);
		}
		/* Check for timeout */
		if (xElapsedTime >= W25QxxFV_TIMEOUT_VALUE) 
		{
			LOG_W25QXX("W25QxxFV Wait TimeOut!\r\n");
			return	W25Qx_TIMEOUT;
		}
		/* Check the value of the register */
		if(status!= W25Qx_OK)
		{
			return W25Qx_ERROR;
		}

    /* Update the address and size variables for next page programming */
    current_addr += current_size;
    pData += current_size;
    current_size = ((current_addr + W25QxxFV_PAGE_SIZE) > end_addr) ? (end_addr - current_addr) : W25QxxFV_PAGE_SIZE;
  } while (current_addr < end_addr);

	return W25Qx_OK;
}

/**
  * @brief  Erases the specified Sector of the QSPI memory. 
  * @param  Address: Sector address to erase
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Erase_OneSector(uint32_t Address)
{
	uint8_t status=0;
	TickType_t xStartTime = xTaskGetTickCount();
	TickType_t xElapsedTime = 0;
	BaseType_t xBusy = pdTRUE;
	
	Address &= ~(W25QxxFV_SECTOR_SIZE - 1);

	/* Enable write operations */
	BSP_W25Qx_Write_Function(WRITE_ENABLE_CMD);
	
	/*Select the FLASH: Chip Select low */
	W25Qx_CS_Enable();
	/* Send the read ID command */
	SPI_ReadWriteByte(SECTOR_ERASE4KB_CMD);
	if (W25Qxx_ID >= W25Q256)
	{
		SPI_ReadWriteByte((uint8_t)(Address >> 24));
	}
	SPI_ReadWriteByte((uint8_t)(Address >> 16));
	SPI_ReadWriteByte((uint8_t)(Address >> 8));
	SPI_ReadWriteByte((uint8_t)(Address));		
	/*Deselect the FLASH: Chip Select high */
	W25Qx_CS_Disable();
	
	/*wait for the W25Qxxx ready*/
	while(xBusy && (xElapsedTime < W25QxxFV_TIMEOUT_VALUE))
	{
		status = BSP_W25Qx_GetStatus(READ_STATUS_REG1_CMD);
		if(status != W25Qx_BUSY)
		{
			xBusy = pdFALSE;
			break;
		}
		xElapsedTime++;
		delay_ms(1);
	}
	/* Check for timeout */
	if (xElapsedTime >= W25QxxFV_TIMEOUT_VALUE) 
	{
		LOG_W25QXX("BSP_W25Qx_Erase_OneSector: W25QxxFV Wait TimeOut!\r\n");
		return	W25Qx_TIMEOUT;
	}
	/* Check the value of the register */
	if(status!= W25Qx_OK)
	{
		return W25Qx_ERROR;
	}

	return W25Qx_OK;
}

/**
  * @brief  Erases the specified Sector of the QSPI memory. 
  * @param  Address: Sector address to erase.
  * @param  Size: Size of data to write Byte
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Erase_MultiSector(uint32_t Address, uint32_t Size)
{
	uint32_t Star_Sector=0,End_Sector=0,Num_Sector=0;
	
	Star_Sector = Address & ~(W25QxxFV_SECTOR_SIZE - 1);
	End_Sector 	= (Address + Size) & ~(W25QxxFV_SECTOR_SIZE - 1);
	Num_Sector 	= (End_Sector - Star_Sector)/W25QxxFV_SECTOR_SIZE + 1;
	
	if(Num_Sector > W25QxxFV_SECTOR_NUMBER)
	{
		LOG_W25QXX("BSP_W25Qx_Erase_MultiSector: Number of sectors exceeds maximum limit!\r\n");
		return W25Qx_ERROR;
	}
	
	for(uint16_t i = 0; i < Num_Sector; i++)
	{
		if(BSP_W25Qx_Erase_OneSector(Address)==W25Qx_OK)
		{
			LOG_W25QXX("BSP_W25Qx_Erase_MultiSector: Erase sector %d success!\r\n", i);
			Address += W25QxxFV_SECTOR_SIZE;
		}
		else
		{
			LOG_W25QXX("BSP_W25Qx_Erase_MultiSector: Erase sector %d failed!\r\n", i);
			return W25Qx_ERROR;
		}
		
	}

	return W25Qx_OK;
}

/**
  * @brief  Erases the entire QSPI memory.This function will take a very long time.
  * @retval QSPI memory status
  */
uint8_t BSP_W25Qx_Erase_Chip(void)
{
	uint8_t status=0;
	TickType_t xStartTime = xTaskGetTickCount();
	TickType_t xElapsedTime = 0;
	BaseType_t xBusy = pdTRUE;

	/* Enable write operations */
	BSP_W25Qx_Write_Function(WRITE_ENABLE_CMD);
	
	/*Select the FLASH: Chip Select low */
	W25Qx_CS_Enable();
	/* Send the read ID command */
	SPI_ReadWriteByte(CHIP_ERASE_CMD);
	/*Deselect the FLASH: Chip Select high */
	W25Qx_CS_Disable();
	
	/*wait for the W25Qxxx ready*/
	while(xBusy && (xElapsedTime < W25QxxFV_TIMEOUT_VALUE))
	{
		status = BSP_W25Qx_GetStatus(READ_STATUS_REG1_CMD);
		if(status != W25Qx_BUSY)
		{
			xBusy = pdFALSE;
			break;
		}
		xElapsedTime++;
		delay_ms(1);
	}
	/* Check for timeout */
	if (xElapsedTime >= W25QxxFV_ERASE_CHIP_TIMEOUT_VALUE) 
	{
		LOG_W25QXX("BSP_W25Qx_Erase_Chip: W25QxxFV wait TimeOut!\r\n");
		return	W25Qx_TIMEOUT;
	}
	/* Check the value of the register */
	if(status!= W25Qx_OK)
	{
		return W25Qx_ERROR;
	}
	
	return W25Qx_OK;
}



