/*
 * @Author: LQS
 * @Date: 2025-04-25 13:59:49
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-07 15:33:47
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\LogConfig\LogConfig.h
 * @Description: 
 */
#ifndef __LOG_CONFIG_H_
#define __LOG_CONFIG_H_
#include <stdio.h>

// 日志总开关
#define LOG_ENABLE 1  // 总开关，设为0禁用所有日志

// 模块日志开关
#define ATCMD_LOG_EN   		    0  // AT命令模块日志
#define FLASH_LOG_EN   		    0  // FLASH模块日志
#define DWT_LOG_EN   		    0  // DWT模块日志

#define SDIO_LOG_EN   		    0  // SDIO模块日志
#define FATFS_LOG_EN   		    0  // FATFS模块日志
#define SD_LOG_EN           	0  // SD卡模块日志
#define SD_API_LOG_EN           0  // SD卡API模块日志

#define TCP_SERVER_LOG_EN   	0  // TCP服务器日志
#define TCP_CLIENT_LOG_EN   	0  // TCP客户端日志
#define UDP_SERVER_LOG_EN       0  // UDP服务器日志
#define UDP_CLIENT_LOG_EN       0  // UDP客户端日志
#define HTTP_LOG_EN   		    1  // HTTP日志
#define MQTT_LOG_EN   		    1  // MQTT日志

#define MBRTU_LOG_EN   		    0  // MBRTU模块日志
#define MBTCP_LOG_EN   		    0  // MBTCP模块日志
#define MBASCII_LOG_EN   	    0  // MBASCII模块日志

#define PCF8574_LOG_EN   	    0  // PCF8574模块日志
#define DHT11_LOG_EN  	        0  // DHT11模块日志
#define W25QXX_LOG_EN  	        0  // W25QXX模块日志



// 日志打印宏
#if LOG_ENABLE
#define LOG_ATCMD(format, ...)          do { if (ATCMD_LOG_EN) printf("[ATCMD] " format, ##__VA_ARGS__); } while (0)
#define LOG_FLASH(format, ...)          do { if (FLASH_LOG_EN) printf("[FLASH] " format, ##__VA_ARGS__); } while (0)
#define LOG_DWT(format, ...)            do { if (DWT_LOG_EN) printf("[DWT] " format, ##__VA_ARGS__); } while (0)

#define LOG_SDIO(format, ...)           do { if (SDIO_LOG_EN) printf("[SDIO] " format, ##__VA_ARGS__); } while (0)
#define LOG_FATFS(format, ...)          do { if (FATFS_LOG_EN) printf("[FATFS] " format, ##__VA_ARGS__); } while (0)
#define LOG_SD(format, ...)             do { if (SD_LOG_EN) printf("[SD] " format, ##__VA_ARGS__); } while (0)
#define LOG_SD_API(format, ...)         do { if (SD_API_LOG_EN) printf("[SD_API] " format, ##__VA_ARGS__); } while (0)

#define LOG_TCP_SERVER(format, ...)     do { if (TCP_SERVER_LOG_EN) printf("[TCP_SERVER] " format, ##__VA_ARGS__); } while (0)
#define LOG_TCP_CLIENT(format, ...)     do { if (TCP_CLIENT_LOG_EN) printf("[TCP_CLIENT] " format, ##__VA_ARGS__); } while (0)
#define LOG_UDP_SERVER(format, ...)     do { if (UDP_SERVER_LOG_EN) printf("[UDP_SERVER] " format, ##__VA_ARGS__); } while (0)
#define LOG_UDP_CLIENT(format, ...)     do { if (UDP_CLIENT_LOG_EN) printf("[UDP_CLIENT] " format, ##__VA_ARGS__); } while (0)
#define LOG_HTTP(format, ...)           do { if (HTTP_LOG_EN) printf("[HTTP] " format, ##__VA_ARGS__); } while (0)
#define LOG_MQTT(format, ...)           do { if (MQTT_LOG_EN) printf("[MQTT] " format, ##__VA_ARGS__); } while (0)

#define LOG_MBRTU(format, ...)          do { if (MBRTU_LOG_EN) printf("[MBRTU] " format, ##__VA_ARGS__); } while (0)
#define LOG_MBTCP(format, ...)          do { if (MBTCP_LOG_EN) printf("[MBTCP] " format, ##__VA_ARGS__); } while (0)
#define LOG_MBASCII(format, ...)        do { if (MBASCII_LOG_EN) printf("[MBASCII] " format, ##__VA_ARGS__); } while (0)

#define LOG_PCF8574(format, ...)        do { if (PCF8574_LOG_EN) printf("[PCF8574] " format, ##__VA_ARGS__); } while (0)
#define LOG_DHT11(format, ...)          do { if (DHT11_LOG_EN) printf("[DHT11] " format, ##__VA_ARGS__); } while (0)
#define LOG_W25QXX(format, ...)         do { if (W25QXX_LOG_EN) printf("[W25QXX] " format, ##__VA_ARGS__); } while (0)

#else
#define LOG_ATCMD(format, ...)          do {} while (0)
#define LOG_FLASH(format, ...)          do {} while (0)
#define LOG_DWT(format, ...)            do {} while (0)

#define LOG_SDIO(format, ...)           do {} while (0)
#define LOG_FATFS(format, ...)          do {} while (0)
#define LOG_SD(format, ...)             do {} while (0)
#define LOG_SD_API(format, ...)         do {} while (0)

#define LOG_TCP_SERVER(format, ...)     do {} while (0)
#define LOG_TCP_CLIENT(format, ...)     do {} while (0)
#define LOG_UDP_SERVER(format, ...)     do {} while (0)
#define LOG_UDP_CLIENT(format, ...)     do {} while (0)
#define LOG_HTTP(format, ...)           do {} while (0)
#define LOG_MQTT(format, ...)           do {} while (0)

#define LOG_MBRTU(format, ...)          do {} while (0)
#define LOG_MBTCP(format, ...)          do {} while (0)
#define LOG_MBASCII(format, ...)        do {} while (0)

#define LOG_PCF8574(format, ...)        do {} while (0)
#define LOG_DHT11(format, ...)          do {} while (0)
#define LOG_W25QXX(format, ...)         do {} while (0)

#endif

#endif



 

