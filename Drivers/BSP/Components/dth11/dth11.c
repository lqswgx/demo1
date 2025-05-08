/*
 * @Author: LQS
 * @Date: 2025-04-24 12:00:15
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-25 17:26:57
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Drivers\BSP\Components\dth11\dth11.c
 * @Description: 
 */
#include "dth11.h"

#define DHT11_TIMEOUT     1000 // 超时时间(us)

static uint8_t DHT11_CheckResponse(void);
static uint8_t DHT11_ReadByte(void);

DHT11_Data_t DHT11_Data;

/**
  * @brief  初始化DHT11
  * @param  None
  * @retval None
  */
void DHT11_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOG_CLK_ENABLE();   
    /* 配置GPIO */
    GPIO_InitStruct.Pin = DHT11_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStruct);
    
    /* 发送开始信号前先拉高DATA线 */
    DHT11_OUT_Init();
    DHT11_PIN_H;
    DHT11_Delay_ms(20);

    LOG_DHT11("DHT11 init success\n");
}

/**
  * @brief  读取DHT11数据
  * @param  dht11: 存放DHT11数据结构体的指针
  * @retval None
  */
void DHT11_ReadData(DHT11_Data_t* dht11)
{
    uint8_t checksum = 0;
    
    /* 发送开始信号 */
    DHT11_OUT_Init();
    DHT11_PIN_L;
    DHT11_Delay_ms(20);  // 保持低电平至少18ms
    
    DHT11_PIN_H;
    DHT11_Delay_us(30);  // 拉高20-40us
    
    /* 切换为输入模式等待DHT11响应 */
    DHT11_IN_Init();
    
    /* 检查DHT11响应 */
    if(!DHT11_CheckResponse())
    {
        LOG_DHT11("DHT11 response error\n");
        return;
    }
    
    /* 读取40位数据 */
    for(int i = 0; i < 5; i++)
    {
        dht11->data[i] = DHT11_ReadByte();
    }
    
    /* 校验数据 */
    checksum = dht11->data[0] + dht11->data[1] + dht11->data[2] + dht11->data[3];
    if(checksum != dht11->data[4])
    {
        LOG_DHT11("DHT11 data checksum error\n");
        return;
    }
    
    //DHT11温湿度
    dht11->humidity = dht11->data[0] + dht11->data[1] * 0.1f;
    dht11->temperature = dht11->data[2] + dht11->data[3] * 0.1f;

    LOG_DHT11("temperature: %.1f\u2103, humidity: %.1f%%RH\n", dht11->temperature, dht11->humidity);
}


/**
 * @brief  检测DHT11的响应
 * @param  无
 * @retval 0：成功，1：失败
 */
static uint8_t DHT11_CheckResponse(void)
{
    uint32_t timeout = DHT11_TIMEOUT;
    
    //等待DHT11响应
    while(DHT11_PIN_READ() != GPIO_PIN_RESET)
    {
        if(timeout-- == 0)
        {
            LOG_DHT11("DHT11 response timeout\n");
            return 0;
        }
        DHT11_Delay_us(1);
    }

    /* 等待DHT11拉低80us结束 */
    while(DHT11_PIN_READ() == GPIO_PIN_RESET)
    {
        if(timeout-- == 0)
        {
            LOG_DHT11("DHT11 response timeout\n");
            return 0;
        }
        DHT11_Delay_us(1);
    }
    
    timeout = DHT11_TIMEOUT;
    
    /* 等待DHT11拉高80us结束 */
    while(DHT11_PIN_READ() == GPIO_PIN_SET)
    {
        if(timeout-- == 0)
        {
            LOG_DHT11("DHT11 response timeout\n");
            return 0;
        }
        DHT11_Delay_us(1);
    }
    
    return 1;
}

/**
 * @brief  读取一个字节数据
 * @param  无
 * @retval 8位数据
 */
static uint8_t DHT11_ReadByte(void)
{
    uint8_t byte = 0;
    uint32_t timeout = DHT11_TIMEOUT;

    for(int i = 0; i < 8; i++)
    {
        /* 等待50us低电平 */
        while(DHT11_PIN_READ() == GPIO_PIN_RESET)
        {
            if(timeout-- == 0)
            {
                LOG_DHT11("DHT11 read byte timeout\n");
                return 0;
            }
            DHT11_Delay_us(1);
        }
        
        /* 测量高电平持续时间 */
        uint32_t start = DWT_GetTicks();
        while(DHT11_PIN_READ() == GPIO_PIN_SET);
        {
            if(timeout-- == 0)
            {
                LOG_DHT11("DHT11 read byte timeout\n");
                return 0;
            }
            DHT11_Delay_us(1);
        }
        uint32_t duration = DWT_TicksToUs(DWT_GetTicks() - start);
        
        /* 判断是0还是1 */
        byte <<= 1;
        if(duration > 40)  // 高电平持续70us表示1，26-28us表示0
        {
            byte |= 1;
        }
    }
    
    return byte;
}
