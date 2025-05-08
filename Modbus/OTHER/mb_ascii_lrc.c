/*
 * @Author: LQS
 * @Date: 2025-05-07 20:30:04
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-08 00:32:31
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\Modbus\OTHER\mb_ascii_lrc.c
 * @Description: 
 */

#include "mb_ascii_lrc.h"

// 将二进制字节转换为 ASCII 字符对（例如 0xA5 -> "A5"）
void byte_to_ascii(uint8_t byte, uint8_t* ascii)
{
    uint8_t high = (byte >> 4) & 0x0F; // 高 4 位
    uint8_t low = byte & 0x0F;         // 低 4 位
    
    // 转换高位和低位为 ASCII（0-9: '0'-'9', 10-15: 'A'-'F'）
    ascii[0] = (high < 10) ? (high + '0') : (high - 10 + 'A');
    ascii[1] = (low < 10) ? (low + '0') : (low - 10 + 'A');
}

// 将 ASCII 字符对转换为二进制字节（例如 "A5" -> 0xA5）
uint8_t ascii_to_byte(const uint8_t* ascii)
{
    uint8_t high = ascii[0];
    uint8_t low = ascii[1];
    
    // 转换高位（0-9: '0'-'9', A-F: 'A'-'F'）
    high = (high >= 'A') ? (high - 'A' + 10) : (high - '0');
    low = (low >= 'A') ? (low - 'A' + 10) : (low - '0');
    
    return (high << 4) | low;
}

// 计算 Modbus ASCII 的 LRC 值（返回二进制值）
uint8_t modbus_ascii_lrc_calc(const uint8_t* ascii_msg, size_t ascii_len)
{
    if (ascii_msg == NULL || ascii_len < 2 || (ascii_len % 2) != 0)
    {
        return 0; // 无效输入
    }

    uint8_t sum = 0;
    // 每两个 ASCII 字符转换为一个二进制字节
    for (size_t i = 0; i < ascii_len; i += 2)
    {
        sum += ascii_to_byte(&ascii_msg[i]);
    }

    // LRC = (和的补码 + 1) 的低 8 位
    return (-sum) & 0xFF;
}

// 验证 Modbus ASCII 报文的 LRC 是否正确
uint8_t modbus_ascii_lrc_verify(const uint8_t* ascii_msg, size_t ascii_len)
{
    if (ascii_msg == NULL || ascii_len < 4 || (ascii_len % 2) != 0)
    {
        return 1; // 无效输入
    }

    // 提取报文数据（不含 LRC）
    size_t data_len = ascii_len - 2; // 最后 2 个字符是 LRC
    uint8_t calc_lrc = modbus_ascii_lrc_calc(ascii_msg, data_len);

    // 提取报文中的 LRC
    uint8_t msg_lrc = ascii_to_byte(&ascii_msg[data_len]);

    // 比较计算的 LRC 和报文中的 LRC
    return (calc_lrc == msg_lrc) ? 0 : 1; // 0 表示校验通过，1 表示失败
}
