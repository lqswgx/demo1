/*
 * @Author: LQS
 * @Date: 2025-04-30 13:14:00
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-04 17:02:11
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\FATFS\App\sd_card.c
 * @Description: 
 */
#include "sd_card.h"
#include "sd_card_api.h"

// 列目录回调函数
static void dir_callback(const char *name, uint8_t is_dir, uint32_t size)
{
    printf("%s %s (%u bytes)\r\n", is_dir ? "DIR" : "FILE", name, size);
}

// SD 卡测试任务
void Sd_Card_Task(void const * argument)
{
    SD_Result res;
    uint8_t read_buffer[256];
    uint32_t bytes_read;
    SD_FileInfo file_info;
    SD_VolumeInfo vol_info;

    // 初始化 SD 卡
    res = sd_init();
    if (res != SD_OK) {
        printf("SD Init Failed: %d\r\n", res);
        vTaskDelete(NULL);
    }

    // //格式化 SD 卡（谨慎使用，会清除所有数据）
    // printf("Formatting SD Card (WARNING: This will erase all data)...\r\n");
    // res = sd_format_card();
    // if (res != SD_OK) {
    //     printf("Format Card Failed: %d\r\n", res);
    // } else {
    //     printf("SD Card Formatted to FAT32\r\n");
    // }    

    // 创建目录
    res = sd_create_dir("test_dir");
    if (res != SD_OK) {
        printf("Create Dir Failed: %d\r\n", res);
    } else {
        printf("Created directory: test_dir\r\n");
    }

    // 测试 .txt 文件
    res = sd_create_txt_file("test_dir/log.txt", "Initial log entry");
    if (res != SD_OK) {
        printf("Create TXT File Failed: %d\r\n", res);
    } else {
        printf("Created log.txt\r\n");
    }

    res = sd_append_txt_file("test_dir/log.txt", "Second log entry");
    if (res != SD_OK) {
        printf("Append TXT File Failed: %d\r\n", res);
    } else {
        printf("Appended to log.txt\r\n");
    }

    res = sd_read_txt_file("test_dir/log.txt", read_buffer, sizeof(read_buffer) - 1, &bytes_read);
    if (res != SD_OK) {
        printf("Read TXT File Failed: %d\r\n", res);
    } else {
        read_buffer[bytes_read] = '\0';
        printf("Read %u bytes from log.txt: %s\r\n", bytes_read, read_buffer);
    }

    // 测试 .csv 文件
    const char *csv_headers[] = {"Time", "Sensor", "Value"};
    res = sd_create_csv_file("test_dir/data.csv", csv_headers, 3);
    if (res != SD_OK) {
        printf("Create CSV File Failed: %d\r\n", res);
    } else {
        printf("Created data.csv\r\n");
    }

    const char *csv_row1[] = {"2025-04-30 12:00:00", "Temp", "25.5"};
    res = sd_append_csv_row("test_dir/data.csv", csv_row1, 3);
    if (res != SD_OK) {
        printf("Append CSV Row Failed: %d\r\n", res);
    } else {
        printf("Appended row to data.csv\r\n");
    }

    const char *csv_row2[] = {"2025-04-30 12:01:00", "Humidity", "60%"};
    res = sd_append_csv_row("test_dir/data.csv", csv_row2, 3);
    if (res != SD_OK) {
        printf("Append CSV Row Failed: %d\r\n", res);
    } else {
        printf("Appended row to data.csv\r\n");
    }

    res = sd_read_csv_file("test_dir/data.csv", read_buffer, sizeof(read_buffer) - 1, &bytes_read);
    if (res != SD_OK) {
        printf("Read CSV File Failed: %d\r\n", res);
    } else {
        read_buffer[bytes_read] = '\0';
        printf("Read %u bytes from data.csv: %s\r\n", bytes_read, read_buffer);
    }

    // 获取文件信息
    res = sd_get_file_info("test_dir/log.txt", &file_info);
    if (res != SD_OK) {
        printf("Get File Info Failed: %d\r\n", res);
    } else {
        printf("log.txt Info: Size=%u bytes, Attrib=0x%02X\r\n", file_info.size, file_info.attrib);
        printf("Created: %04d-%02d-%02d %02d:%02d:%02d\r\n",
               1980 + (file_info.create_date >> 9),
               (file_info.create_date >> 5) & 0x0F,
               file_info.create_date & 0x1F,
               file_info.create_time >> 11,
               (file_info.create_time >> 5) & 0x3F,
               (file_info.create_time & 0x1F) * 2);
    }

    // 设置文件属性（设为只读）
    res = sd_set_file_attrib("test_dir/log.txt", AM_RDO);
    if (res != SD_OK) {
        printf("Set File Attrib Failed: %d\r\n", res);
    } else {
        printf("Set log.txt to read-only\r\n");
    }

    // 复制文件
    res = sd_copy_file("test_dir/log.txt", "test_dir/log_copy.txt");
    if (res != SD_OK) {
        printf("Copy File Failed: %d\r\n", res);
    } else {
        printf("Copied log.txt to log_copy.txt\r\n");
    }

    // 重命名文件
    res = sd_rename_file("test_dir/log_copy.txt", "test_dir/log_renamed.txt");
    if (res != SD_OK) {
        printf("Rename File Failed: %d\r\n", res);
    } else {
        printf("Renamed log_copy.txt to log_renamed.txt\r\n");
    }

    // 获取卷信息
    res = sd_get_volume_info(&vol_info);
    if (res != SD_OK) {
        printf("Get Volume Info Failed: %d\r\n", res);
    } else {
        printf("Volume Info: Total=%llu bytes, Free=%llu bytes, Type=%s\r\n",
               vol_info.total_bytes, vol_info.free_bytes,
               vol_info.card_type == 1 ? "SDSC" : vol_info.card_type == 2 ? "SDHC" : "SDXC");
    }

    // 列出目录
    res = sd_list_dir("test_dir", dir_callback);
    if (res != SD_OK) {
        printf("List Dir Failed: %d\r\n", res);
    }

    // // 删除文件
    // res = sd_delete_file("test_dir/log.txt");
    // if (res != SD_OK) {
    //     printf("Delete File Failed: %d\r\n", res);
    // } else {
    //     printf("Deleted file: test_dir/log.txt\r\n");
    // }

    // res = sd_delete_file("test_dir/log_renamed.txt");
    // if (res != SD_OK) {
    //     printf("Delete File Failed: %d\r\n", res);
    // } else {
    //     printf("Deleted file: test_dir/log_renamed.txt\r\n");
    // }

    // res = sd_delete_file("test_dir/data.csv");
    // if (res != SD_OK) {
    //     printf("Delete File Failed: %d\r\n", res);
    // } else {
    //     printf("Deleted file: test_dir/data.csv\r\n");
    // }

    // 卸载 SD 卡
    res = sd_deinit();
    if (res != SD_OK) {
        printf("SD Deinit Failed: %d\r\n", res);
    } else {
        printf("SD Card Unmounted\r\n");
    }

    vTaskDelete(NULL);
}

// void create_csv_file(void) {
//     FATFS fs;
//     FIL file;
//     FRESULT res;

//     // 挂载文件系统
//     res = f_mount(&fs, "", 1);
//     if (res != FR_OK) return;

//     // 打开文件
//     res = f_open(&file, "data.csv", FA_CREATE_ALWAYS | FA_WRITE);
//     if (res != FR_OK) {
//         f_mount(0, "", 0);
//         return;
//     }

//     // 定义字段宽度
//     int time_width = 20; // 时间字段宽度
//     int temp_width = 12; // 温度字段宽度
//     int hum_width = 10;  // 湿度字段宽度

//     // 写入表头
//     f_printf(&file, "%-*s,%-*s,%-*s\n", 
//              time_width, "Time", 
//              temp_width, "Temperature", 
//              hum_width, "Humidity");

//     // 写入数据
//     f_printf(&file, "%-*s,%-*s,%-*s\n", 
//              time_width, "2025-05-03 10:00:00", 
//              temp_width, "25.5", 
//              hum_width, "60%");
//     f_printf(&file, "%-*s,%-*s,%-*s\n", 
//              time_width, "2025-05-03 10:01:00", 
//              temp_width, "26.0", 
//              hum_width, "58%");

//     // 关闭文件和卸载
//     f_close(&file);
//     f_mount(0, "", 0);
// }
