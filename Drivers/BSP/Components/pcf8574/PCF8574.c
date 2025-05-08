/*
 * @Author: LQS
 * @Date: 2025-04-15 18:31:00
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-06 10:05:01
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Drivers\BSP\Components\pcf8574\PCF8574.c
 * @Description: 
 */
#include "PCF8574.h"

/**
 * @brief  Initializes the PCF8574 I/O expander.
 * @param  None
 * @retval None
 */
void PCF8574_Init(void) 
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();	

  /*Configure GPIO pin : PB12 */
  GPIO_InitStruct.Pin = PCF8574_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(PCF8574_INT_GPIO_Port, &GPIO_InitStruct);

  IIC_Init(); //初始化IIC
  IIC_Start(); //启动IIC
  IIC_SendByte(PCF8574_ADDR+W_Bit); //发送器件地址
  uint8_t ack = IIC_WaitAck(); //等待ACK
  IIC_Stop(); //停止IIC
  if (ack == 0) 
  {
    PCF8574_WriteByte(0xFF); //发送数据,默认设置为0xFF
    LOG_PCF8574("PCF8574 init success\n");
  } 
  else
  {
    LOG_PCF8574("PCF8574 init failed\n");
  }
}

/**
 * @brief  Writes a byte to the PCF8574 I/O expander.
 * @param  data: The byte to be written.
 * @retval None
 */
void PCF8574_WriteByte(uint8_t data)
{
  IIC_Start(); //启动IIC
  IIC_SendByte(PCF8574_ADDR+W_Bit); //发送器件地址
  IIC_WaitAck(); //等待ACK
  IIC_SendByte(data); //发送数据
  IIC_WaitAck(); //等待ACK
  IIC_Stop(); //停止IIC
}

/**
 * @brief  Reads a byte from the PCF8574 I/O expander.
 * @param  None
 * @retval The byte read from the device.
 */
uint8_t PCF8574_ReadByte(void)
{
  uint8_t data = 0;
  IIC_Start(); //启动IIC
  IIC_SendByte(PCF8574_ADDR+R_Bit); //发送器件地址
  IIC_WaitAck(); //等待ACK
  data = IIC_ReadByte(); //读取数据
  IIC_NAck(); //发送NACK
  IIC_Stop(); //停止IIC
  return data; //返回读取到的数据
}

/**
 * @brief  Writes a bit to a specific position in the PCF8574 I/O expander.
 * @param  bit: The bit position (0-7).
 * @param  value: The value to be written (0 or 1).
 * @retval None
 */
void PCF8574_WriteBit(uint8_t bit, uint8_t value)
{
  uint8_t data = PCF8574_ReadByte(); //读取当前数据
  if (value) 
  {
    data |= (1 << bit); //设置指定位置1
  } 
  else 
  {
    data &= ~(1 << bit); //设置指定位置0
  }
  PCF8574_WriteByte(data); //写入数据
}

/**
 * @brief  Reads a bit from a specific position in the PCF8574 I/O expander.
 * @param  bit: The bit position (0-7).
 * @retval The value read from the specified bit (0 or 1).
 */
uint8_t PCF8574_ReadBit(uint8_t bit)
{
  uint8_t data = PCF8574_ReadByte(); //读取当前数据
  return (data >> bit) & 0x01; //返回指定位置的值
}

/**
 * @brief  Resets the PHY (Physical Layer) of the Ethernet module.
 * @param  None
 * @retval None
 */
void PHY_Reset(void)
{
  PCF8574_WriteBit(ETH_RESET_IO, 1); //复位PHY
  PCF8574_Delay(100); //延时
  PCF8574_WriteBit(ETH_RESET_IO, 0); //取消复位
  PCF8574_Delay(100); //延时
}

void RS485_Dir_Set(uint8_t dir)
{
  PCF8574_WriteBit(RS485_RE_IO, dir&0x01); //0-接收 1-发送
}
