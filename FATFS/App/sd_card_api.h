/*
 * @Author: LQS
 * @Date: 2025-04-30 15:37:30
 * @LastEditors: LQS
 * @LastEditTime: 2025-04-30 16:44:47
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\FATFS\App\sd_card_api.h
 * @Description: 
 */
#ifndef __SD_CARD_API_H_
#define __SD_CARD_API_H_
#include "fatfs.h"
#include "main.h"
#include "us_freertos.h"

// SD 卡操作结果
typedef enum {
    SD_OK = 0,            // 成功
    SD_ERR_INIT,          // 初始化失败
    SD_ERR_MOUNT,         // 挂载失败
    SD_ERR_OPEN,          // 打开文件/目录失败
    SD_ERR_WRITE,         // 写入失败
    SD_ERR_READ,          // 读取失败
    SD_ERR_DELETE,        // 删除失败
    SD_ERR_NO_SPACE,      // 空间不足
    SD_ERR_INVALID_PARAM, // 参数无效
    SD_ERR_OTHER          // 其他错误
} SD_Result;

// 文件信息结构体
typedef struct {
    uint32_t size;        // 文件大小（字节）
    uint16_t create_date; // 创建日期（FatFs 格式）
    uint16_t create_time; // 创建时间（FatFs 格式）
    uint16_t modify_date; // 修改日期
    uint16_t modify_time; // 修改时间
    uint8_t attrib;       // 文件属性（AM_RDO, AM_HID, AM_SYS, AM_DIR 等）
} SD_FileInfo;

// 卷信息结构体
typedef struct {
    uint64_t total_bytes; // 总容量（字节）
    uint64_t free_bytes;  // 可用容量（字节）
    uint8_t card_type;    // 卡类型（0=未知, 1=SDSC, 2=SDHC, 3=SDXC）
} SD_VolumeInfo;

// 初始化 SD 卡并挂载文件系统
SD_Result sd_init(void);

// 卸载文件系统
SD_Result sd_deinit(void);

// 创建或覆盖指定文件
SD_Result sd_create_file(const char *path);

// 创建指定目录
SD_Result sd_create_dir(const char *path);

// 写入数据到指定文件（mode: 0=覆盖, 1=追加）
SD_Result sd_write_file(const char *path, const uint8_t *data, uint32_t size, uint8_t mode);

// 读取指定文件内容到缓冲区
SD_Result sd_read_file(const char *path, uint8_t *buffer, uint32_t size, uint32_t *bytes_read);

// 删除指定文件或目录
SD_Result sd_delete_file(const char *path);

// 获取 SD 卡剩余空间（单位：字节）
SD_Result sd_get_free_space(uint64_t *free_bytes);

// 列出指定目录下的文件和子目录
SD_Result sd_list_dir(const char *path, void (*callback)(const char *name, uint8_t is_dir, uint32_t size));

// 格式化 SD 卡为 FAT32
SD_Result sd_format_card(void);

// 重命名或移动文件/目录
SD_Result sd_rename_file(const char *old_path, const char *new_path);

// 复制文件到新路径
SD_Result sd_copy_file(const char *src_path, const char *dst_path);

// 获取文件信息
SD_Result sd_get_file_info(const char *path, SD_FileInfo *info);

// 追加数据到文件
SD_Result sd_append_file(const char *path, const uint8_t *data, uint32_t size);

// 截断文件到指定大小
SD_Result sd_truncate_file(const char *path, uint32_t size);

// 设置文件属性
SD_Result sd_set_file_attrib(const char *path, uint8_t attrib);

// 获取卷信息
SD_Result sd_get_volume_info(SD_VolumeInfo *info);

// 创建或覆盖 .txt 文件
SD_Result sd_create_txt_file(const char *path, const char *initial_content);

// 追加文本到 .txt 文件
SD_Result sd_append_txt_file(const char *path, const char *content);

// 读取 .txt 文件内容
SD_Result sd_read_txt_file(const char *path, uint8_t *buffer, uint32_t size, uint32_t *bytes_read);

// 创建或覆盖 .csv 文件（写入表头）
SD_Result sd_create_csv_file(const char *path, const char **headers, uint32_t header_count);

// 追加一行到 .csv 文件
SD_Result sd_append_csv_row(const char *path, const char **fields, uint32_t field_count);

// 读取 .csv 文件内容
SD_Result sd_read_csv_file(const char *path, uint8_t *buffer, uint32_t size, uint32_t *bytes_read);
#endif


 

