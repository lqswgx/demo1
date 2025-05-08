##作者：lqs
##日期：2025-05-08

1.简介
    这是一个基于正点原子Apollo开发板STM32F429IGT6的demo，采用STM32CUBEMX工具配置工程，使用了FreeRTOS，包括各种接口驱动、LWIP协议栈、FATFS文件系统、Modbus自定义协议栈等。

2.主要接口
- USART1：用于通过ttl模块与PC进行通信，波特率115200，8N1, 采用串口轮询方式打印日志信息
- USART2：用于RS485通信，波特率9600，8N1, 采用DMA+空闲中断，实现Modbus协议（自定义，非FreeModbus）
- USART3: 用于AT指令调试外部串口模块，波特率115200，8N1, 采用串口接收中断和发送轮询方式
- 模拟IIC: 用于驱动PCF8574, 实现端口扩展
- SPI: 用于驱动W25Q256, 实现扩展外部FLASH
- 以太网口：用于实现TCP/IP协议，实现外部网络通信
- SDIO: 用于驱动外部SD卡

3.协议栈
- LWIP: 实现了TCP CLENT/SERVER 和 UDP的demo任务，其中TCP CLENT实现了Modbus-TCP自定义协议栈
- Modbus: 个人编写，实现了ASCII、RTU、TCP的从机协议栈，支持功能码01、02、03、04、05、06、0f、10

4.文件系统
- FATFS: 采用SDIO接口，实现了文件读写、目录创建、删除等API接口

5.其他
- 日志控制：各个模块单独定义日志打印函数，可自主开关
