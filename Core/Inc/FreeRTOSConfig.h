/* USER CODE BEGIN Header */
/*
 * FreeRTOS Kernel V10.3.1
 * Portion Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Portion Copyright (C) 2019 StMicroelectronics, Inc.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
/* USER CODE END Header */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * These parameters and more are described within the 'configuration' section of the
 * FreeRTOS API documentation available on the FreeRTOS.org web site.
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/

/* USER CODE BEGIN Includes */
/* Section where include file can be added */
/* USER CODE END Includes */

/* Ensure definitions are only used by the compiler, and not by the assembler. */
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
  #include <stdint.h>
  extern uint32_t SystemCoreClock;
#endif
#define configENABLE_FPU                         0
#define configENABLE_MPU                         0
 
// 配置是否使用抢占式调度
#define configUSE_PREEMPTION                     1
// 配置是否支持静态内存分配
#define configSUPPORT_STATIC_ALLOCATION          0
// 配置是否支持动态内存分配
#define configSUPPORT_DYNAMIC_ALLOCATION         1
// 配置是否使用空闲任务钩子函数
#define configUSE_IDLE_HOOK                      0
// 配置是否使用时钟节拍钩子函数
#define configUSE_TICK_HOOK                      0
// 配置CPU时钟频率
#define configCPU_CLOCK_HZ                       ( SystemCoreClock )
// 配置时钟节拍频率
#define configTICK_RATE_HZ                       ((TickType_t)1000)
// 配置任务最大优先级数量
#define configMAX_PRIORITIES                     ( 32 )
// 配置最小堆栈大小
#define configMINIMAL_STACK_SIZE                 ((uint16_t)128)
// 配置总的堆内存大小
#define configTOTAL_HEAP_SIZE                    ((size_t)50*1024) //50KB
// 配置任务名称最大长度
#define configMAX_TASK_NAME_LEN                  ( 16 )
// 配置是否使用16位时钟节拍计数器
#define configUSE_16_BIT_TICKS                   0
// 配置是否使用互斥锁
#define configUSE_MUTEXES                        1
// 配置队列注册表最大大小
#define configQUEUE_REGISTRY_SIZE                16
// 配置是否使用递归互斥锁
#define configUSE_RECURSIVE_MUTEXES              1
// 配置是否使用计数信号量
#define configUSE_COUNTING_SEMAPHORES            1
// 配置是否使用优化的任务选择方法
#define configUSE_PORT_OPTIMISED_TASK_SELECTION  1
 
// 配置堆栈溢出检查级别
#define configCHECK_FOR_STACK_OVERFLOW           2
 
// 配置是否使用跟踪功能
#define configUSE_TRACE_FACILITY									1
// 配置是否使用统计格式化函数
#define configUSE_STATS_FORMATTING_FUNCTIONS			1

/* USER CODE BEGIN MESSAGE_BUFFER_LENGTH_TYPE */
/* Defaults to size_t for backward compatibility, but can be changed
   if lengths will always be less than the number of bytes in a size_t. */

// 定义消息缓冲区长度的数据类型为 size_t
#define configMESSAGE_BUFFER_LENGTH_TYPE         size_t

/* USER CODE END MESSAGE_BUFFER_LENGTH_TYPE */

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES                    0  // 是否启用协程功能，0 表示禁用
#define configMAX_CO_ROUTINE_PRIORITIES          ( 2 )  // 协程的最大优先级数

/* Software timer definitions. */
// 定义是否启用FreeRTOS的定时器功能
#define configUSE_TIMERS                         1
// 设置定时器任务的优先级
#define configTIMER_TASK_PRIORITY                ( 2 )
// 设置定时器队列的长度 
#define configTIMER_QUEUE_LENGTH                 16
// 设置定时器任务的堆栈深度 
#define configTIMER_TASK_STACK_DEPTH             256

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

// 定义是否包含任务优先级设置函数
#define INCLUDE_vTaskPrioritySet             1
// 定义是否包含获取任务优先级的函数 
#define INCLUDE_uxTaskPriorityGet            1
// 定义是否包含任务删除函数 
#define INCLUDE_vTaskDelete                  1
// 定义是否包含清理任务资源的函数 
#define INCLUDE_vTaskCleanUpResources        0
// 定义是否包含任务挂起函数 
#define INCLUDE_vTaskSuspend                 1
// 定义是否包含任务延迟直到指定时间点的函数 
#define INCLUDE_vTaskDelayUntil              1
// 定义是否包含任务延迟函数 
#define INCLUDE_vTaskDelay                   1
// 定义是否包含获取任务调度器状态的函数 
#define INCLUDE_xTaskGetSchedulerState       1
// 定义是否包含获取任务堆栈高水位标记的函数 
#define INCLUDE_uxTaskGetStackHighWaterMark	 1

/* Cortex-M specific definitions. */
#ifdef __NVIC_PRIO_BITS
 /* __BVIC_PRIO_BITS will be specified when CMSIS is being used. */
// 定义 FreeRTOS 任务优先级位数为 NVIC 中断优先级位数
 #define configPRIO_BITS         __NVIC_PRIO_BITS
#else
 #define configPRIO_BITS         4
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
// 定义库使用的最低中断优先级
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY   15

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
// 定义系统调用可以被中断的最大中断优先级
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all Cortex-M ports, and do not rely on any particular library functions. */
// 定义内核中断优先级，通过将最低中断优先级左移（8 - configPRIO_BITS）位来实现
#define configKERNEL_INTERRUPT_PRIORITY 		( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
// 定义系统调用的最大中断优先级，通过将库的最大系统调用中断优先级左移（8 - configPRIO_BITS）位来计算
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/* Normal assert() semantics without relying on the provision of an assert.h
header file. */
/* USER CODE BEGIN 1 */
// 宏定义：configASSERT 用于在断言失败时禁用中断并进入无限循环
#define configASSERT( x ) if ((x) == 0) {taskDISABLE_INTERRUPTS(); for( ;; );}

/* USER CODE END 1 */

/* Definitions that map the FreeRTOS port interrupt handlers to their CMSIS
standard names. */
#define vPortSVCHandler    SVC_Handler
// 定义vPortSVCHandler为SVC_Handler，用于指定FreeRTOS在发生SVC调用时调用的函数
 
#define xPortPendSVHandler PendSV_Handler
// 定义xPortPendSVHandler为PendSV_Handler，用于指定FreeRTOS在发生PendSV调用时调用的函数

/* IMPORTANT: This define is commented when used with STM32Cube firmware, when the timebase source is SysTick,
              to prevent overwriting SysTick_Handler defined within STM32Cube HAL */

#define xPortSysTickHandler SysTick_Handler
// 定义xPortSysTickHandler为SysTick_Handler的别名

/* USER CODE BEGIN Defines */
/* Section where parameter definitions can be added (for instance, to override default ones in FreeRTOS.h) */
/* USER CODE END Defines */

#endif /* FREERTOS_CONFIG_H */
