/*
 * @Author: LQS
 * @Date: 2025-05-05 22:33:36
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-06 11:02:46
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\FreeModbus\App\mb_func_callback.c
 * @Description: 
 */
#include "mb_func_callback.h"

uint16_t usRegInputBuf[INPUT_REG_COUNT];
uint16_t usRegHoldingBuf[HOLDING_REG_COUNT];
uint8_t	ucCoilBuf[COIL_COUNT / 8];
uint8_t	ucDiscreteBuf[DISCRETE_COUNT / 8];

SemaphoreHandle_t xRegMutex;

void MX_Modbus_Init(void)
{
    eMBErrorCode eStatus;

    xRegMutex = xSemaphoreCreateMutex();
    if (xRegMutex == NULL)
    {
        printf("Create Mutex failed\r\n");
        while (1);
    }
    xSemaphoreGive(xRegMutex);
    
	InitInputRegisters();
	InitHoldingRegisters();
	InitCoils();
	InitDiscreteInputs();

    // 初始化 FreeModbus 从机，地址为 1，波特率 9600
    eStatus = eMBInit(MB_RTU, 0x01, 2, 9600, MB_PAR_NONE);
    if (eStatus != MB_ENOERR) {
        // 初始化失败处理
        printf("Modbus init failed: %d\r\n", eStatus);
        while (1);
    }
    
    // 启用 Modbus
    eStatus = eMBEnable();
    if (eStatus != MB_ENOERR)
    {
        // 启用失败处理
        printf("Modbus enable failed: %d\r\n", eStatus);
        while (1);
    }    
}

void InitInputRegisters(void)
{
    for (int i = 0; i < INPUT_REG_COUNT; i++)
	{
        usRegInputBuf[i] = 0;
    }
}

void InitHoldingRegisters(void)
{
    for (int i = 0; i < HOLDING_REG_COUNT; i++)
	{
        usRegHoldingBuf[i] = 0;
    }
}

void InitCoils(void)
{
    for (int i = 0; i < COIL_COUNT / 8; i++)
	{
        ucCoilBuf[i] = 0;
    }
}

void InitDiscreteInputs(void)
{
    for (int i = 0; i < DISCRETE_COUNT / 8; i++)
	{
        ucDiscreteBuf[i] = 0;
    }
}

eMBErrorCode eMBRegInputCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex = usAddress - 1;

    /* 获取互斥量 */
    if (xSemaphoreTake(xRegMutex, portMAX_DELAY) == pdTRUE) {
        if ((iRegIndex >= 0) && (iRegIndex + usNRegs <= INPUT_REG_COUNT)) {
            while (usNRegs > 0) {
                *pucRegBuffer++ = (UCHAR)(usRegInputBuf[iRegIndex] >> 8);
                *pucRegBuffer++ = (UCHAR)(usRegInputBuf[iRegIndex] & 0xFF);
                iRegIndex++;
                usNRegs--;
            }
        } else {
            eStatus = MB_ENOREG; /* 地址越界 */
        }
        /* 释放互斥量 */
        xSemaphoreGive(xRegMutex);
    } else {
        eStatus = MB_EIO; /* 互斥量获取失败 */
    }

    return eStatus;
}


eMBErrorCode eMBRegHoldingCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iRegIndex = usAddress - 1;

    if (xSemaphoreTake(xRegMutex, portMAX_DELAY) == pdTRUE) {
        if ((iRegIndex >= 0) && (iRegIndex + usNRegs <= HOLDING_REG_COUNT)) {
            if (eMode == MB_REG_READ) {
                while (usNRegs > 0) {
                    *pucRegBuffer++ = (UCHAR)(usRegHoldingBuf[iRegIndex] >> 8);
                    *pucRegBuffer++ = (UCHAR)(usRegHoldingBuf[iRegIndex] & 0xFF);
                    iRegIndex++;
                    usNRegs--;
                }
            } else if (eMode == MB_REG_WRITE) {
                while (usNRegs > 0) {
                    usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                    usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                    iRegIndex++;
                    usNRegs--;
                }
            }
        } else {
            eStatus = MB_ENOREG;
        }
        xSemaphoreGive(xRegMutex);
    } else {
        eStatus = MB_EIO;
    }

    return eStatus;    
}


eMBErrorCode eMBRegCoilsCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode)
{
eMBErrorCode eStatus = MB_ENOERR;
    int iCoilIndex = usAddress - 1;
    int iByteIndex, iBitOffset;

    if (xSemaphoreTake(xRegMutex, portMAX_DELAY) == pdTRUE) {
        if ((iCoilIndex >= 0) && (iCoilIndex + usNCoils <= COIL_COUNT)) {
            if (eMode == MB_REG_READ) {
                while (usNCoils > 0) {
                    UCHAR ucResult = 0;
                    int iBits = (usNCoils > 8) ? 8 : usNCoils;
                    for (int i = 0; i < iBits; i++) {
                        iByteIndex = (iCoilIndex + i) / 8;
                        iBitOffset = (iCoilIndex + i) % 8;
                        if (ucCoilBuf[iByteIndex] & (1 << iBitOffset)) {
                            ucResult |= (1 << i);
                        }
                    }
                    *pucRegBuffer++ = ucResult;
                    iCoilIndex += iBits;
                    usNCoils -= iBits;
                }
            } else if (eMode == MB_REG_WRITE) {
                while (usNCoils > 0) {
                    UCHAR ucByte = *pucRegBuffer++;
                    int iBits = (usNCoils > 8) ? 8 : usNCoils;
                    for (int i = 0; i < iBits; i++) {
                        iByteIndex = (iCoilIndex + i) / 8;
                        iBitOffset = (iCoilIndex + i) % 8;
                        if (ucByte & (1 << i)) {
                            ucCoilBuf[iByteIndex] |= (1 << iBitOffset);
                        } else {
                            ucCoilBuf[iByteIndex] &= ~(1 << iBitOffset);
                        }
                    }
                    iCoilIndex += iBits;
                    usNCoils -= iBits;
                }
            }
        } else {
            eStatus = MB_ENOREG;
        }
        xSemaphoreGive(xRegMutex);
    } else {
        eStatus = MB_EIO;
    }

    return eStatus;
}


eMBErrorCode eMBRegDiscreteCB(UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete)
{
    eMBErrorCode eStatus = MB_ENOERR;
    int iDiscreteIndex = usAddress - 1;

    if (xSemaphoreTake(xRegMutex, portMAX_DELAY) == pdTRUE) {
        if ((iDiscreteIndex >= 0) && (iDiscreteIndex + usNDiscrete <= DISCRETE_COUNT)) {
            while (usNDiscrete > 0) {
                UCHAR ucResult = 0;
                int iBits = (usNDiscrete > 8) ? 8 : usNDiscrete;
                for (int i = 0; i < iBits; i++) {
                    int iByteIndex = (iDiscreteIndex + i) / 8;
                    int iBitOffset = (iDiscreteIndex + i) % 8;
                    if (ucDiscreteBuf[iByteIndex] & (1 << iBitOffset)) {
                        ucResult |= (1 << i);
                    }
                }
                *pucRegBuffer++ = ucResult;
                iDiscreteIndex += iBits;
                usNDiscrete -= iBits;
            }
        } else {
            eStatus = MB_ENOREG;
        }
        xSemaphoreGive(xRegMutex);
    } else {
        eStatus = MB_EIO;
    }

    return eStatus;
}
