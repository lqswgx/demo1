/*
 * @Author: LQS
 * @Date: 2025-03-27 16:22:18
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-08 20:08:45
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Core\Src\us_freertos.c
 * @Description: 
 */

#include "us_freertos.h"

//define the task handles
osThreadId DefaultTaskHandle;           //默认任务句柄       
osThreadId DI_FILTER_TASKHANDLE;        //DI滤波任务句柄      
osThreadId DO_CONTROL_TASKHANDLE;       //DO控制任务句柄     
osThreadId CAN1_TASKHandle;             //CAN1任务句柄           
osThreadId USART1_TASKHandle;           //USART1任务句柄         
osThreadId USART2_TASKHandle;           //USART2任务句柄        
osThreadId USART3_TASKHandle;           //USART3任务句柄         
osThreadId SYSTEM_WAKEUP_TASKHandle;    //系统唤醒任务句柄  
osThreadId IWDG_TASKHandle;             //IWDG任务句柄           
osThreadId HEALTH_MONITOR_TASKHandle;   //健康监控任务句柄 
osThreadId TCP_SERVER_TASKHandle;       //TCP服务器任务句柄     
osThreadId TCP_CLIENT_TASKHandle;       //TCP客户端任务句柄
osThreadId UDP_SERVER_TASKHandle;       //UDP服务器任务句柄     
osThreadId UDP_CLIENT_TASKHandle;       //UDP客户端任务句柄
osThreadId WIFI_STA_TASKHandle;         //WIFI STA任务句柄       
osThreadId WIFI_AP_TASKHandle;          //WIFI AP任务句柄        
osThreadId SD_CARD_TASKHandle;          //SD卡任务句柄          
osThreadId MBRTU_SLAVE_TASKHandle;      //MODBUS从机任务句柄     

//define the timer handles
osTimerId myTimer01Handle;              //定时器句柄
/* Private function prototypes -----------------------------------------------*/

static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
/**
 * @brief  Get memory allocation for the Idle task.
 * @param  ppxIdleTaskTCBBuffer: Pointer to the TCB buffer for the Idle task.
 * @param  ppxIdleTaskStackBuffer: Pointer to the stack buffer for the Idle task.
 * @param  pulIdleTaskStackSize: Pointer to the size of the stack for the Idle task.
 * @retval None
 * @note   This function is called by FreeRTOS during initialization.
 */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &xIdleStack[0];
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];
/**
 * @brief  Get memory allocation for the Timer task.
 * @param  ppxTimerTaskTCBBuffer: Pointer to the TCB buffer for the Timer task.
 * @param  ppxTimerTaskStackBuffer: Pointer to the stack buffer for the Timer task.
 * @param  pulTimerTaskStackSize: Pointer to the size of the stack for the Timer task.
 * @retval None
 * @note   This function is called by FreeRTOS during initialization.
 */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
    *ppxTimerTaskStackBuffer = &xTimerStack[0];
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
    /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
 * @brief  Stack overflow hook function.
 * @param  None
 * @retval None
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
  printf("Stack overflow in task: %s\n", pcTaskName);
  while (1); 
}

/**
 * @brief  Malloc failed hook function.
 * @param  None
 * @retval None
 */
void vApplicationMallocFailedHook(void) {
    printf("Malloc failed\n");
    while (1);
}

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void)
{
    /* Create the timer(s) */
    /* definition and creation of myTimer01 */
    // osTimerDef(myTimer01, Callback01);
    // myTimer01Handle = osTimerCreate(osTimer(myTimer01), osTimerPeriodic, NULL);

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityLow, 0, 2048);
    DefaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    // /* definition and creation of CAN1_TASK */
    // osThreadDef(CAN1_TASK, Can1_Task, osPriorityNormal, 0, 128);
    // CAN1_TASKHandle = osThreadCreate(osThread(CAN1_TASK), NULL);

    // /* definition and creation of USART1_TASK */
    // osThreadDef(USART1_TASK, Usart1_Task, osPriorityNormal, 0, 128);
    // USART1_TASKHandle = osThreadCreate(osThread(USART1_TASK), NULL);

    // /* definition and creation of USART2_TASK */
    // osThreadDef(USART2_TASK, Usart2_Task, osPriorityNormal, 0, 128);
    // USART2_TASKHandle = osThreadCreate(osThread(USART2_TASK), NULL);

    // /* definition and creation of USART3_TASK */
    // osThreadDef(USART3_TASK, Uasrt3_Task, osPriorityNormal, 0, 128);
    // USART3_TASKHandle = osThreadCreate(osThread(USART3_TASK), NULL);

    // /* definition and creation of SYSTEM_WAKEUP_TASK */
    // osThreadDef(SYSTEM_WAKEUP_TASK, System_Wakeup_Task, osPriorityHigh, 0, 256);
    // SYSTEM_WAKEUP_TASKHandle = osThreadCreate(osThread(SYSTEM_WAKEUP_TASK), NULL);
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
// extern lan8742_Object_t LAN8742;
void StartDefaultTask(void const * argument)
{
    DIDO_Event_t DI_Event;

    uint8_t buffer1[512]={0};
    // uint32_t status = 0;
    // int32_t PHYLinkState = 0;

    // uint8_t buffer[512] = {0};
    // const uint32_t blockCount = 1; // 示例：读写 1024 块（512 KB）
    // const uint32_t eraseStart = 0;    // 擦除起始块
    // const uint32_t eraseEnd = 0;   // 擦除结束块（擦除 1024 块）

    FLASH_Init(&flash_handle, &device_info);
    AT_Cmd_Init(&at_cmd_instance, &AT_UART_HANDLE);
    // BSP_W25Qx_Init();
    PCF8574_Init();
    // MX_LWIP_Init();
    // DHT11_Init();

    osThreadDef(DI_FILTER_TASK, DI_Filter_Task, osPriorityAboveNormal, 0, 256);
    DI_FILTER_TASKHANDLE = osThreadCreate(osThread(DI_FILTER_TASK), NULL);

    osThreadDef(DO_CONTROL_TASK, DO_Control_Task, osPriorityNormal, 0, 256);
    DO_CONTROL_TASKHANDLE = osThreadCreate(osThread(DO_CONTROL_TASK), NULL);
    
    // osThreadDef(TCP_CLIENT_TASK, Tcp_Client_Task, osPriorityNormal, 0, 2048);
    // TCP_CLIENT_TASKHandle = osThreadCreate(osThread(TCP_CLIENT_TASK), NULL);

    // osThreadDef(TCP_SERVER_TASK, Tcp_Server_Task, osPriorityNormal, 0, 2048);
    // TCP_SERVER_TASKHandle = osThreadCreate(osThread(TCP_SERVER_TASK), NULL);

    // osThreadDef(UDP_CLIENT_TASK, Udp_Client_Task, osPriorityNormal, 0, 1024);
    // UDP_CLIENT_TASKHandle = osThreadCreate(osThread(UDP_CLIENT_TASK), NULL);

    // osThreadDef(UDP_SERVER_TASK, Udp_Server_Task, osPriorityNormal, 0, 2048);
    // UDP_SERVER_TASKHandle = osThreadCreate(osThread(UDP_SERVER_TASK), NULL);

    // osThreadDef(SD_CARD_TASK, Sd_Card_Task, osPriorityNormal, 0, 2048);
    // SD_CARD_TASKHandle = osThreadCreate(osThread(SD_CARD_TASK), NULL);

    // osThreadDef(MBRTU_SLAVE_TASK, Mbrtu_Slave_Task, osPriorityHigh, 0, 1024);
    // MBRTU_SLAVE_TASKHandle = osThreadCreate(osThread(MBRTU_SLAVE_TASK), NULL);

    // osDelay(1000);
    /* Infinite loop */
    for(;;)
    { 
        // AT_Cmd_Send(&at_cmd_instance,"AT+UARTCFG?",AT_DEFAULT_CRLF);
        // DHT11_ReadData(&DHT11_Data);
        // PHYLinkState = LAN8742_GetLinkState(&LAN8742);
        // switch (PHYLinkState)
        // {
        // case LAN8742_STATUS_100MBITS_FULLDUPLEX:
        //     printf("ethernet_link_thread:PHYLNK: 100M FullDuplex\n");
        //     break;
        // case LAN8742_STATUS_100MBITS_HALFDUPLEX:
        //     printf("ethernet_link_thread:PHYLNK: 100M HalfDuplex\n");
        //     break;
        // case LAN8742_STATUS_10MBITS_FULLDUPLEX:
        //     printf("ethernet_link_thread:PHYLNK: 10M FullDuplex\n");
        //     break;
        // case LAN8742_STATUS_10MBITS_HALFDUPLEX:
        //     printf("ethernet_link_thread:PHYLNK: 10M HalfDuplex\n");
        //     break;
        // }

        // HAL_ETH_ReadPHYRegister(&heth, LAN8720A_PHY_ADDRESS, 0, &status);
        // printf("PHY Register0: 0x%04x\n", status);
        // HAL_ETH_ReadPHYRegister(&heth, LAN8720A_PHY_ADDRESS, 1, &status);
        // printf("PHY Register1: 0x%04x\n", status);
        // HAL_ETH_ReadPHYRegister(&heth, LAN8720A_PHY_ADDRESS, 2, &status);
        // printf("PHY Register2: 0x%04x\n", status);
        // HAL_ETH_ReadPHYRegister(&heth, LAN8720A_PHY_ADDRESS, 3, &status);
        // printf("PHY Register3: 0x%04x\n", status);     

        if(xQueueReceive(DI_Event_QueueHandle, &DI_Event, 0) == pdTRUE)
        {
            if(DI_Event.channel==0)
            {
                if(DI_Event.new_state==0)
                {
                    memset(&device_info, 0x00, sizeof(device_info));
                    device_info.major = 0x01;
                    device_info.minor = 0x00;
                    device_info.patch = 0x00;
                    FLASH_StoreDeviceInfo(&flash_handle, &device_info);
                    // // 获取 SD 卡信息
                    // if (SD_GetCardInfo(&SDCardInfo) == HAL_OK)
                    // {
                    //     printf("SD Card Info:\n");
                    //     printf("  Type: %s\n", SDCardInfo.card_type);
                    //     printf("  Capacity: %llu bytes\n", SDCardInfo.capacity);
                    //     printf("  Block Size: %d bytes\n", SDCardInfo.block_size);
                    //     printf("  Block Count: %d\n", SDCardInfo.block_count);
                    // }
                    // // 擦除指定块范围
                    // if (SD_EraseBlocks(eraseStart, eraseEnd) == HAL_OK)
                    // {
                    //     printf("Erase blocks %d to %d succeeded\n", eraseStart, eraseEnd);
                    // }
                    // // 验证擦除（读取擦除后的数据）
                    // memset(buffer, 0x00, sizeof(buffer));
                    // if (SD_ReadBlocks(eraseStart, buffer, blockCount) == HAL_OK)
                    // {
                    //     printf("Read erased blocks succeeded\n");
                    //     // 检查前几个字节（擦除后通常为 0xFF 或 0x00）
                    //     for (int i = 0; i < 16; i++) {
                    //         printf("%02X ", buffer[i]);
                    //     }
                    //     printf("\n");
                    // }
                    // // 准备写入数据
                    // memset(buffer, 0xAA, sizeof(buffer));
                    // // 写入多块
                    // if (SD_WriteBlocks(eraseStart, buffer, blockCount) == HAL_OK)
                    // {
                    //     printf("Write %d blocks succeeded\n", blockCount);
                    // }
                    // // 清空缓冲区
                    // memset(buffer, 0x00, blockCount * SDCardInfo.block_size);
                    // // 读取多块
                    // if (SD_ReadBlocks(eraseStart, buffer, blockCount) == HAL_OK)
                    // {
                    //     printf("Read %d blocks succeeded\n",blockCount);
                    //     // 检查写入的数据
                    //     for (int i = 0; i < 16; i++)
                    //     {
                    //         printf("%02X ", buffer[i]);
                    //     }
                    //     printf("\n");
                    // }
                    // FLASH_VerifyDeviceInfo(&flash_handle, &device_info);
                    // printf("Device Info: data:\n");
                    // for(int i=0;i<DEVICE_DATA_SIZE;i++)
                    // {
                    //     printf("0x%02x\t", device_info.data[i]);
                    // }
                    // printf("\n");
                    // printf("Device Info: constant:0x%02x\n",device_info.constant);
                    // printf("Device Info: checksum:0x%04x\n",device_info.checksum);                    
                    // SysTime.year=2025;
                    // SysTime.month=4;
                    // SysTime.day=28;
                    // SysTime.hours=23;
                    // SysTime.minutes=30;
                    // SysTime.seconds=0;
                    // SysTime.weekday=1;
                    // Set_RTCTimeAndDate(&SysTime);
                }
                else
                {
                    SysTime = Get_RTCTimeAndDate();
                    printf("Current SysTime:%d-%02d-%02d %02d:%02d:%02d\r\n", SysTime.year, SysTime.month, SysTime.day, SysTime.hours, SysTime.minutes, SysTime.seconds);            
                    osThreadList(buffer1);//print task list
                    printf("Task List:\n%s\t\r\n", buffer1);
                }
            }
            else if(DI_Event.channel==1)
            {
                if(DI_Event.new_state==0)
                {
                    // device_info.data[0] = 0x11;
                    // device_info.data[1] = 0x22;
                    // device_info.data[2] = 0x33;
                    // FLASH_StoreDeviceInfo(&flash_handle, &device_info);
                }
                else
                {
                    
                }
            }
            else if(DI_Event.channel==2)
            {
                if(DI_Event.new_state==0)
                {
                    SysTime = Get_RTCTimeAndDate();
                    printf("Current SysTime:%d-%02d-%02d %02d:%02d:%02d\r\n", SysTime.year, SysTime.month, SysTime.day, SysTime.hours, SysTime.minutes, SysTime.seconds);            
                }
                else
                {
                    osThreadList(buffer1);//print task list
                    printf("Task List:\n%s\t\r\n", buffer1);
                }
            }
        }

        osDelay(1000);
    }
    /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Can1_Task */
/**
* @brief Function implementing the CAN1_TASK thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Can1_Task */
void Can1_Task(void const * argument)
{
    /* USER CODE BEGIN Can1_Task */
    /* Infinite loop */
    for(;;)
    {
        osDelay(1000);
    }
    /* USER CODE END Can1_Task */
}

/* Callback01 function */
void Callback01(void const * argument)
{
    /* USER CODE BEGIN Callback01 */

    /* USER CODE END Callback01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
