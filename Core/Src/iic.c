/*
 * @Author: LQS
 * @Date: 2025-04-15 13:45:48
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-24 11:13:31
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Core\Src\iic.c
 * @Description: 
 */
#include "iic.h"

/**
 * @brief  IIC初始化函数
 * @param  None
 * @retval None
 */
void IIC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();		

  /*Configure GPIO pins : IIC_SCL_Pin IIC_SDA_Pin */
  GPIO_InitStruct.Pin = IIC_SCL_Pin|IIC_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  IIC_SDA_H;
  IIC_SCL_H;
}

/**
 * @brief  IIC启动信号
 * @param  None
 * @retval None
 */
void IIC_Start(void)
{
  SDA_OUT_Init(); //设置SDA为输出模式
  IIC_SDA_H; //SDA高
  IIC_SCL_H; //SCL高
  IIC_Delay(4); //延时us
  IIC_SDA_L;
  IIC_Delay(4); //延时us
  IIC_SCL_L;
}

/**
 * @brief  IIC停止信号
 * @param  None
 * @retval None
 */
void IIC_Stop(void)
{
  SDA_OUT_Init(); //设置SDA为输出模式
  IIC_SDA_L; //SDA低
  IIC_SCL_L; //SCL低
  IIC_Delay(4); //延时us
  IIC_SDA_H;
  IIC_Delay(4); //延时us
  IIC_SCL_H;
}

/**
 * @brief  IIC发送字节
 * @param  byte: 要发送的字节
 * @retval None
 */
void IIC_SendByte(uint8_t byte)
{
  uint8_t i;
  SDA_OUT_Init(); //设置SDA为输出模式
  IIC_SCL_L; //SCL低

  for(i=0; i<8; i++)
  {
    if(byte & 0x80) //发送高位
    {
      IIC_SDA_H; //SDA高
    }
    else
    {
      IIC_SDA_L; //SDA低
    }
    IIC_Delay(2); //延时us
    IIC_SCL_H; //SCL高
    IIC_Delay(2); //延时us
    IIC_SCL_L; //SCL低
    IIC_Delay(2); //延时us
    byte <<= 1; //左移一位，准备发送下一个字节
  }
}

/**
 * @brief  IIC读取字节
 * @param  None
 * @retval 读取到的字节
 */
uint8_t IIC_ReadByte(void)
{
  uint8_t i, byte = 0;
  SDA_IN_Init(); //设置SDA为输入模式

  for(i=0; i<8; i++)
  {
    IIC_SCL_L; //SCL低
    IIC_Delay(2); //延时us
    IIC_SCL_H; //SCL高
    byte <<= 1; //左移一位，准备接收下一个字节
    if(IIC_SDA_READ()) //读取SDA状态
    {
      byte |= 0x01; //如果SDA为高，则设置最低位为1
    }
    IIC_Delay(1); //延时us
  }
  return byte;
}

/**
 * @brief  IIC等待ACK信号
 * @param  None
 * @retval 0: ACK接收成功, 1: 超时
 */
uint8_t IIC_WaitAck(void)
{
  uint8_t timeout = 0;
  SDA_IN_Init(); //设置SDA为输入模式
  IIC_SDA_H; //SDA高
  IIC_Delay(1); //延时us
  IIC_SCL_H; //SCL高
  IIC_Delay(1); //延时us

  while(IIC_SDA_READ()) //等待ACK信号
  {
    timeout++;
    if(timeout > 250) //超时处理
    {
      IIC_Stop();
      return 1;
    }
  }
  IIC_SCL_L; //SCL低
  return 0;
}

/**
 * @brief  IIC发送ACK信号
 * @param  None
 * @retval None
 */
void IIC_Ack(void)
{
  IIC_SCL_L; //SCL低
  SDA_OUT_Init(); //设置SDA为输出模式
  IIC_SDA_L; //SDA低
  IIC_Delay(2); //延时us
  IIC_SCL_H; //SCL高
  IIC_Delay(2); //延时us
  IIC_SCL_L; //SCL低
}

/**
 * @brief  IIC发送NACK信号
 * @param  None
 * @retval None
 */
void IIC_NAck(void)
{
  IIC_SCL_L; //SCL低
  SDA_OUT_Init(); //设置SDA为输出模式
  IIC_SDA_H; //SDA高
  IIC_Delay(2); //延时us
  IIC_SCL_H; //SCL高
  IIC_Delay(2); //延时us
  IIC_SCL_L; //SCL低
}
