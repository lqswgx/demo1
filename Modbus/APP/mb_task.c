/*
 * @Author: LQS
 * @Date: 2025-05-06 10:30:14
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 23:38:59
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\APP\mb_task.c
 * @Description: 
 */

#include "mb_task.h"
#include "mb_init.h"

#include "rtu_slave.h"
#include "rtu_master.h"
#include "tcp_slave.h"
#include "tcp_master.h"
#include "ascii_slave.h"
#include "ascii_master.h"

#include "gpio.h"

void Mbrtu_Slave_Task(void const * argument)
{
	mb_init();
	
    while (1)
    {
		for(uint8_t i=0;i<DI_CHANNEL_COUNT;i++)
        {
            DiscreteBuff[i] = di_channels[i].stable_state;
        }
		
        for(uint8_t i=0;i<DO_CHANNEL_COUNT;i++)
        {
            CoilBuff[i] =  do_channels[i].current_state;
        }

        if(mb_protocol == MB_RTU)
        {
            rtu_slave_poll();
        }
        else if(mb_protocol == MB_TCP)
        {
            tcp_slave_poll();
        }
        else if(mb_protocol == MB_ASCII)
        {
            ascii_slave_poll();
        }
        

        osDelay(10);
    }    
}
