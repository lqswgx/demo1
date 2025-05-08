/*
 * @Author: LQS
 * @Date: 2025-05-06 10:30:14
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-06 15:17:45
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\FreeModbus\App\mb_task.c
 * @Description: 
 */
#include "mb_task.h"

void Mbrtu_Slave_Task(void const * argument)
{
    uint16_t counter = 0; /* 用于模拟温度 */
    uint8_t buttonState = 0; /* 模拟按钮状态 */
    uint8_t ledState = 0; /* 模拟 LED 状态 */
    TickType_t lastWakeTime = xTaskGetTickCount();

    MX_Modbus_Init(); /* 初始化 Modbus */

    while (1)
    {
        if (xSemaphoreTake(xRegMutex, portMAX_DELAY) == pdTRUE)
        {
            /* 更新输入寄存器：模拟温度（地址 1） */
            usRegInputBuf[0] = counter % 1000; /* 0-999 循环 */
            counter += 10; /* 每次增加 10 */

            /* 更新离散输入：模拟按钮（地址 1） */
            if ((xTaskGetTickCount() - lastWakeTime) >= pdMS_TO_TICKS(1000)) {
                buttonState = !buttonState;
                ucDiscreteBuf[0] = (buttonState ? 1 : 0); /* 切换状态 */
            }

            /* 更新线圈：模拟 LED（地址 1） */
            if ((xTaskGetTickCount() - lastWakeTime) >= pdMS_TO_TICKS(2000)) {
                ledState = !ledState;
                ucCoilBuf[0] = (ledState ? 1 : 0); /* 切换状态 */
            }

            /* 检查保持寄存器：打印主站写入的值（地址 1） */
            if ((xTaskGetTickCount() - lastWakeTime) >= pdMS_TO_TICKS(1000)) {
                // printf("Holding Register[1] = %u\n", usRegHoldingBuf[0]);
            }

            xSemaphoreGive(xRegMutex);
        }

        eMBPoll();
        osDelay(100);
    }    
}
