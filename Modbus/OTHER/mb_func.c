/*
 * @Author: LQS
 * @Date: 2025-05-07 18:25:32
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-08 01:55:14
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\OTHER\mb_func.c
 * @Description: 
 */
#include "mb_init.h"

uint8_t error_flag;
uint8_t error_cmd;
uint8_t error_code;

// 函数指针数组，索引对应功能码
ModbusFuncHandler modbus_handlers[17] =
{
    mb_error_handler,           // 错误处理
    mb_func_01_handler,         // 功能码 01: Read Coils
    mb_func_02_handler,         // 功能码 02: Read Discrete Inputs
    mb_func_03_handler,         // 功能码 03: Read Holding Registers
    mb_func_04_handler,         // 功能码 04: Read Input Registers
    mb_func_05_handler,         // 功能码 05: Write Single Coil
    mb_func_06_handler,         // 功能码 06: Write Single Register
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 07-0E 预留
    mb_func_0F_handler,         // 功能码 0F: Write Multiple Coils
    mb_func_10_handler          // 功能码 10: Write Multiple Registers
};

uint8_t mb_rx_check_handle(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len)
{
	uint16_t rx_length,rx_staraddr,rx_val;
	uint16_t RxCRC,RtuCRC;
	uint8_t func_code;

	if(mb_protocol == MB_RTU)
	{
		func_code=rx_buff[1];
		rx_staraddr=(rx_buff[2]<<8)|rx_buff[3];
		rx_length=(rx_buff[4]<<8)|rx_buff[5];
	}	
	else if(mb_protocol == MB_TCP)
	{
		func_code=rx_buff[7];
		rx_staraddr=(rx_buff[8]<<8)|rx_buff[9];
		rx_length=(rx_buff[10]<<8)|rx_buff[11];
	}
	else if(mb_protocol == MB_ASCII)
	{
		func_code=ascii_to_byte(rx_buff+3);
		rx_staraddr=(ascii_to_byte(rx_buff+5)<<8)|ascii_to_byte(rx_buff+7);
		rx_length=(ascii_to_byte(rx_buff+9)<<8)|ascii_to_byte(rx_buff+11);
	}
	
    switch(func_code)
	{
		case 0x01:	
			if((rx_staraddr+rx_length)>COIL_COUNT)
			{
				error_flag=1;
				error_cmd=0x80+func_code;
				error_code=0x02;
			} 
			break;

		case 0x02:
			if((rx_staraddr+rx_length)>DISCRETE_COUNT)
			{
				error_flag=1;
				error_cmd=0x80+func_code;
				error_code=0x02;
			}
			break;

		case 0x03:
			if((rx_staraddr+rx_length)>HOLDING_REG_COUNT)
			{
				error_flag=1;
				error_cmd=0x80+func_code;
				error_code=0x02;
			} 
			break;

		case 0x04:
			if((rx_staraddr+rx_length)>INPUT_REG_COUNT)
			{
				error_flag=1;
				error_cmd=0x80+func_code;
				error_code=0x02;
			}
			break;

		case 0x05:
			rx_val=rx_length;
			if((rx_staraddr)>COIL_COUNT)
			{
				error_flag=1;
				error_cmd=0x80+func_code;
				error_code=0x02;
			} 
            else if (rx_val!=0xFF00 && rx_val!=0)
			{
				error_flag=1;
				error_cmd=0x80+func_code;
				error_code=0x03;
			} 
			break;

		case 0x06:
			if((rx_staraddr)>HOLDING_REG_COUNT)
			{
				error_flag=1;
				error_cmd=0x80+func_code;
				error_code=0x02;
			} 
			break;

		case 0x0f:
			if((rx_staraddr+rx_length)>COIL_COUNT)
			{
				error_flag=1;
				error_cmd=0x80+func_code;
				error_code=0x02;
			} 
			break;

		case 0x10:
			if((rx_staraddr)>HOLDING_REG_COUNT)
			{
				error_flag=1;
				error_cmd=0x80+func_code;
				error_code=0x02;
			} 		
			break;

		default:
			error_flag=1;
			error_cmd=func_code;
			error_code=0x01;
			break;
	}
	
	if(error_flag)
	{
		if(mb_protocol == MB_RTU)
		{
			return mb_error_handler(rx_buff,rx_len,tx_buff,tx_len);
		}
		else if(mb_protocol == MB_TCP)
		{
			uint8_t status =  mb_error_handler(rx_buff+6,rx_len-6,tx_buff+6,tx_len);
			
			tx_buff[0] = rx_buff[0];
			tx_buff[1] = rx_buff[1];
			tx_buff[2] = rx_buff[2];
			tx_buff[3] = rx_buff[3];
			tx_buff[4] = ((*tx_len)>>8)&0xff;
			tx_buff[5] = (*tx_len)&0xff;
			*tx_len += 6;

			return status;
		}
		else if(mb_protocol == MB_ASCII)
		{
			return mb_error_handler(rx_buff,rx_len,tx_buff,tx_len);
		}
	}
	
    if(mb_protocol == MB_RTU)
    {
        RtuCRC=CRC16(rx_buff,rx_len-2);
        RxCRC=rx_buff[rx_len-1]<<8|rx_buff[rx_len-2];
        if(RtuCRC!=RxCRC)
        {
            error_flag=1;
            error_cmd=0x55;
            error_code=0x55;
			return mb_error_handler(rx_buff,rx_len,tx_buff,tx_len);
        }
		return modbus_handlers[func_code](rx_buff,rx_len,tx_buff,tx_len); 
    }
	else if(mb_protocol == MB_TCP)
	{
		uint8_t status =  modbus_handlers[func_code](rx_buff+6,rx_len-6,tx_buff+6,tx_len);

		tx_buff[0] = rx_buff[0];
		tx_buff[1] = rx_buff[1];
		tx_buff[2] = rx_buff[2];
		tx_buff[3] = rx_buff[3];
		tx_buff[4] = ((*tx_len)>>8)&0xff;
		tx_buff[5] = (*tx_len)&0xff;
		*tx_len += 6;
		
		return status;
	}
	else if(mb_protocol == MB_ASCII)
	{
		//0-success,1-fail
		if(modbus_ascii_lrc_verify(rx_buff+1,rx_len-3))
		{
            error_flag=1;
            error_cmd=0x55;
            error_code=0x55;
			
			return mb_error_handler(rx_buff,rx_len,tx_buff,tx_len);
		}

		uint16_t ascii_rx_len = (rx_len-3)/2;
		uint8_t ascii_rx_buff[ascii_rx_len];
		memset(ascii_rx_buff,0,ascii_rx_len);

		for(uint16_t i=0;i<ascii_rx_len;i++)
		{
			ascii_rx_buff[i] = ascii_to_byte(rx_buff+i*2+1);
		}

		uint16_t ascii_tx_len = 0;
		uint8_t ascii_tx_buff[512];
		memset(ascii_tx_buff,0,512);

		uint8_t status = modbus_handlers[func_code](ascii_rx_buff,ascii_rx_len,ascii_tx_buff,&ascii_tx_len);

		*tx_len = ascii_tx_len*2+3;
		tx_buff[0] = rx_buff[0];
		tx_buff[*tx_len-2] = '\r';
		tx_buff[*tx_len-1] = '\n';
		for(uint16_t i=0;i<ascii_tx_len;i++)
		{
			byte_to_ascii(ascii_tx_buff[i],tx_buff+i*2+1);
		}

		return status;
	}
	else
	{
		return 0;
	}
}

uint8_t mb_error_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len)
{
	uint8_t i=0;
	error_flag=0;

	if(mb_protocol == MB_ASCII)
	{
		tx_buff[i++]=rx_buff[0];
		tx_buff[i++]=rx_buff[1];
		tx_buff[i++]=rx_buff[2];
		byte_to_ascii(error_cmd,tx_buff+i);
		i+=2;
		byte_to_ascii(error_code,tx_buff+i);
		i+=2;
		tx_buff[i++]='\r';
		tx_buff[i++]='\n';
		
		*tx_len=i;
	}
	else if(mb_protocol == MB_RTU || mb_protocol == MB_TCP)
	{
		tx_buff[i++]=rx_buff[0];
		tx_buff[i++]=error_cmd;
		tx_buff[i++]=error_code;
		
		*tx_len=i;
	}
	else
	{
		return 0;
	}
			
	return  1;
}

uint8_t mb_func_01_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len)
{
	uint16_t OilStatrAddr,OilNum,Oilbyte,i=0,j,crc;
	uint8_t lrc=0;

	OilStatrAddr=rx_buff[2]<<8|rx_buff[3];
	OilNum=rx_buff[4]<<8|rx_buff[5];
	Oilbyte= (OilNum+7)/8;
	
	tx_buff[i++]=rx_buff[0];
	tx_buff[i++]=0x01;    
	tx_buff[i++]=Oilbyte; 

	bool_array_to_uint8(CoilBuff+OilStatrAddr,OilNum,tx_buff+i,&j);
	i+=j;

    if(mb_protocol == MB_RTU)
    {
        crc = CRC16(tx_buff,i);
        tx_buff[i++]=crc&0xff;
        tx_buff[i++]=(crc>>8)&0xff;
    }
	else if(mb_protocol == MB_ASCII)
	{
		for(j=0;j<i;j++)
		{
			lrc+=tx_buff[j];
		}
		lrc = (-lrc)&0xff;
		tx_buff[i++]=lrc;
	}

	*tx_len=i;
			
	return  1;	 
}

uint8_t mb_func_02_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len)
{
	uint16_t DiscreteStatrAddr,DiscreteNum,Discretebyte,i=0,j,crc;
	uint8_t lrc=0;

	DiscreteStatrAddr=rx_buff[2]<<8|rx_buff[3];
	DiscreteNum=rx_buff[4]<<8|rx_buff[5];
	Discretebyte= (DiscreteNum+7)/8;
	
	tx_buff[i++]=rx_buff[0];
	tx_buff[i++]=0x02;    
	tx_buff[i++]=Discretebyte; 

	bool_array_to_uint8(DiscreteBuff+DiscreteStatrAddr,DiscreteNum,tx_buff+i,&j);
	i+=j;	
	
    if(mb_protocol == MB_RTU)
    {
        crc = CRC16(tx_buff,i);
        tx_buff[i++]=crc&0xff;
        tx_buff[i++]=(crc>>8)&0xff;
    }
	else if(mb_protocol == MB_ASCII)
	{
		for(j=0;j<i;j++)
		{
			lrc+=tx_buff[j];
		}
		lrc = (-lrc)&0xff;
		tx_buff[i++]=lrc;
	}
	*tx_len=i;
			
	return  1;
}

uint8_t mb_func_03_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len)
{	
	uint16_t HoldResStartAddr,HoldResNum,Holdbyte,i=0,j,crc;
	uint8_t lrc=0;

	HoldResStartAddr=rx_buff[2]<<8|rx_buff[3];
	HoldResNum=rx_buff[4]<<8|rx_buff[5];
	Holdbyte=HoldResNum*2;
	
	tx_buff[i++] = rx_buff[0];  
	tx_buff[i++]=0x03;               
	tx_buff[i++]=Holdbyte&0xff;
	
	for(j=0;j<HoldResNum;j++)
	{
		tx_buff[i++]=(HoldingRegs[HoldResStartAddr+j]>>8)&0xff;
		tx_buff[i++]=HoldingRegs[HoldResStartAddr+j]&0xff;
	}
    if(mb_protocol == MB_RTU)
    {
        crc = CRC16(tx_buff,i);
        tx_buff[i++]=crc&0xff;
        tx_buff[i++]=(crc>>8)&0xff;
    }
	else if(mb_protocol == MB_ASCII)
	{
		for(j=0;j<i;j++)
		{
			lrc+=tx_buff[j];
		}
		lrc = (-lrc)&0xff;
		tx_buff[i++]=lrc;
	}	
	*tx_len=i;	 	 

	return  1;
}

uint8_t mb_func_04_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len)
{
	uint16_t InputResStartAddr,InputResNum,Inputbyte,i=0,j,crc;
	uint8_t lrc=0;

	InputResStartAddr=rx_buff[2]<<8|rx_buff[3];
	InputResNum=rx_buff[4]<<8|rx_buff[5];
	Inputbyte=InputResNum*2;
	
	tx_buff[i++] = rx_buff[0];  
	tx_buff[i++]=0x04;               
	tx_buff[i++]=Inputbyte&0xff;
	
	for(j=0;j<InputResNum;j++)
	{
		tx_buff[i++]=(InputRegs[InputResStartAddr+j]>>8)&0xff;
		tx_buff[i++]=InputRegs[InputResStartAddr+j]&0xff;
	}
    if(mb_protocol == MB_RTU)
    {
        crc = CRC16(tx_buff,i);
        tx_buff[i++]=crc&0xff;
        tx_buff[i++]=(crc>>8)&0xff;
    }
	else if(mb_protocol == MB_ASCII)
	{
		for(j=0;j<i;j++)
		{
			lrc+=tx_buff[j];
		}
		lrc = (-lrc)&0xff;
		tx_buff[i++]=lrc;
	}	
	*tx_len=i;	 	 

	return  1;
}

uint8_t mb_func_05_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len)
{
	uint16_t OilStatrAddr,OilVal,Val,j=0;
	uint8_t lrc=0;

	OilStatrAddr=rx_buff[2]<<8|rx_buff[3];
	OilVal      =rx_buff[4]<<8|rx_buff[5];
	Val= OilVal==0xFF00?1:0;
  
	CoilBuff[OilStatrAddr]=Val;
	
	// for(i=0;i<8;i++)tx_buff[i] = rx_buff[i];

    if(mb_protocol == MB_RTU)
    {
		memcpy(tx_buff,rx_buff,8);
        *tx_len=8;
    }
	else if(mb_protocol == MB_TCP)
    {
		memcpy(tx_buff,rx_buff,6);
        *tx_len=6;
    }
	else if(mb_protocol == MB_ASCII)
	{
		memcpy(tx_buff,rx_buff,6);
		for(j=0;j<6;j++)
		{
			lrc+=tx_buff[j];
		}
		lrc = (-lrc)&0xff;
		tx_buff[6]=lrc;
		*tx_len=7;
	}

	return  1;	 
}

uint8_t mb_func_06_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len)
{
	uint16_t HoldResStartAddr,HoldResVal,j=0;
	uint8_t lrc=0;

	HoldResStartAddr=rx_buff[2]<<8|rx_buff[3];
	HoldResVal=rx_buff[4]<<8|rx_buff[5];
	
    HoldingRegs[HoldResStartAddr]=HoldResVal;
	
	// for(i=0;i<8;i++)tx_buff[i] = rx_buff[i];

    if(mb_protocol == MB_RTU)
    {
		memcpy(tx_buff,rx_buff,8);
        *tx_len=8;
    }
	else if(mb_protocol == MB_TCP)
    {
		memcpy(tx_buff,rx_buff,6);
        *tx_len=6;
    }
	else if(mb_protocol == MB_ASCII)
	{
		memcpy(tx_buff,rx_buff,6);
		for(j=0;j<6;j++)
		{
			lrc+=tx_buff[j];
		}
		lrc = (-lrc)&0xff;
		tx_buff[6]=lrc;
		*tx_len=7;
	}	
	
	return  1;
}

uint8_t mb_func_0F_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len)
{	
	uint16_t OilStatrAddr,OilNum,Oilbyte,i=0,crc,j;
	uint8_t lrc=0;

	OilStatrAddr=rx_buff[2]<<8|rx_buff[3];
	OilNum      =rx_buff[4]<<8|rx_buff[5];
	Oilbyte		=rx_buff[6];

	uint8_to_bool_array(&rx_buff[7],Oilbyte,CoilBuff+OilStatrAddr,&OilNum);
		
	tx_buff[i++]=rx_buff[0];
	tx_buff[i++]=0x0f;
	tx_buff[i++]=rx_buff[2];
	tx_buff[i++]=rx_buff[3];
	tx_buff[i++]=rx_buff[4];
	tx_buff[i++]=rx_buff[5];

    if(mb_protocol == MB_RTU)
    {
        crc = CRC16(tx_buff,i);
        tx_buff[i++]=crc&0xff;
        tx_buff[i++]=(crc>>8)&0xff;
    }
	else if(mb_protocol == MB_ASCII)
	{
		for(j=0;j<i;j++)
		{
			lrc+=tx_buff[j];
		}
		lrc = (-lrc)&0xff;
		tx_buff[i++]=lrc;
	}	
	*tx_len=i;

	return  1;			
}

uint8_t mb_func_10_handler(uint8_t* rx_buff, uint8_t rx_len, uint8_t* tx_buff, uint16_t *tx_len)
{
	uint16_t HoldResStartAddr,HoldResNum,Holdbyte,i=0,j,k=0,crc;
	uint8_t lrc=0;

	HoldResStartAddr=rx_buff[2]<<8|rx_buff[3];
	HoldResNum      =rx_buff[4]<<8|rx_buff[5];
	Holdbyte        =rx_buff[6];
	
	for(j=0;j<HoldResNum;j++)
	{
		HoldingRegs[HoldResStartAddr+j]=rx_buff[7+k]<<8|rx_buff[8+k];
		k+=2;
		if(k>=Holdbyte)break;
	}
			
	tx_buff[i++]=rx_buff[0];
	tx_buff[i++]=0x10;
	tx_buff[i++]=rx_buff[2];
	tx_buff[i++]=rx_buff[3];
	tx_buff[i++]=rx_buff[4];
	tx_buff[i++]=rx_buff[5];
    
    if(mb_protocol == MB_RTU)
    {
        crc = CRC16(tx_buff,i);
        tx_buff[i++]=crc&0xff;
        tx_buff[i++]=(crc>>8)&0xff;
    }
	else if(mb_protocol == MB_ASCII)
	{
		for(j=0;j<i;j++)
		{
			lrc+=tx_buff[j];
		}
		lrc = (-lrc)&0xff;
		tx_buff[i++]=lrc;
	}	
	*tx_len=i;

	return  1;			
}



