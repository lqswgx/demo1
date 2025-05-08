/*
 * @Author: LQS
 * @Date: 2025-04-30 15:37:23
 * @LastEditors: LQS
 * @LastEditTime: 2025-05-04 12:30:20
 * @FilePath: \MDK-ARMe:\LQS\FreeRTOS\demo1\FATFS\App\sd_card_api.c
 * @Description: 
 */
#include "sd_card_api.h"

// 静态变量
static FATFS fs;              // 文件系统对象
static SemaphoreHandle_t sd_sem; // FatFs 同步信号量
static uint8_t is_initialized = 0; // 初始化标志

// 复制操作的缓冲区大小
#define COPY_BUFFER_SIZE 4096
#define CSV_BUFFER_SIZE 512 // CSV 格式化缓冲区

#define WAIT_GIVESEMAPHORE_TIMEOUT  portMAX_DELAY

// 内部函数：创建同步信号量
static SD_Result create_sync_semaphore(void)
{
    sd_sem = xSemaphoreCreateBinary();
    if(sd_sem == NULL)
    {
        LOG_SD_API("Failed to create sync semaphore\r\n");
        return SD_ERR_OTHER;
    }

    return SD_OK;
}

// 初始化 SD 卡并挂载文件系统
SD_Result sd_init(void)
{
    FRESULT res;

    if (is_initialized)
    {
        LOG_SD_API("SD card already initialized\r\n");
        return SD_OK;
    }

    // 创建同步信号量
    if (create_sync_semaphore() != SD_OK)
    {
        return SD_ERR_INIT;
    }

    // 挂载文件系统
    res = f_mount(&fs, "", 1); // 强制挂载
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to mount file system\r\n");
        vSemaphoreDelete( sd_sem );
        return SD_ERR_MOUNT;
    }
    else
    {
        LOG_SD_API("success to mount file system\r\n");
    }

    is_initialized = 1;
    xSemaphoreGive(sd_sem);
    return SD_OK;
}

// 卸载文件系统
SD_Result sd_deinit(void)
{
    FRESULT res;

    if (!is_initialized)
    {
        LOG_SD_API("SD card not initialized\r\n");
        return SD_OK;
    }

    // 卸载文件系统
    res = f_mount(NULL, "", 0);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to unmount file system\r\n");
        return SD_ERR_OTHER;
    }
    else
    {
        LOG_SD_API("success to unmount file system\r\n");
    }


    is_initialized = 0;
    vSemaphoreDelete( sd_sem );
    return SD_OK;
}

// 创建或覆盖指定文件
SD_Result sd_create_file(const char *path)
{
    FIL file;
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!path || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to create file: %s\r\n", path);
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OPEN;
    }
    else
    {
        LOG_SD_API("success to create file: %s\r\n", path);
        f_close(&file);
    }

    xSemaphoreGive(sd_sem);
    return ret;
}

// 创建指定目录
SD_Result sd_create_dir(const char *path)
{
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!path || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_mkdir(path);
    if (res != FR_OK && res != FR_EXIST)
    {
        LOG_SD_API("Failed to create directory: %s\r\n", path);
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OTHER;
    }
    else
    {
        LOG_SD_API("success to create directory: %s\r\n", path);
    }

    xSemaphoreGive(sd_sem);
    return ret;
}

// 写入数据到指定文件
SD_Result sd_write_file(const char *path, const uint8_t *data, uint32_t size, uint8_t mode)
{
    FIL file;
    FRESULT res;
    UINT bw;
    SD_Result ret = SD_OK;

    if (!path || !data || size == 0 || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    // 选择写入模式：覆盖或追加
    uint8_t flags = (mode == 1) ? (FA_OPEN_APPEND | FA_WRITE) : (FA_CREATE_ALWAYS | FA_WRITE);
    res = f_open(&file, path, flags);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to open file\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OPEN;
        goto exit;
    }

    res = f_write(&file, data, size, &bw);
    if (res != FR_OK || bw != size)
    {
        LOG_SD_API("Failed to write data\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_WRITE;
    }
    else
    {
        LOG_SD_API("Write %d bytes\r\n", bw);
    }

    f_close(&file);
exit:
    xSemaphoreGive(sd_sem);
    return ret;
}

// 读取指定文件内容
SD_Result sd_read_file(const char *path, uint8_t *buffer, uint32_t size, uint32_t *bytes_read)
{
    FIL file;
    FRESULT res;
    UINT br;
    SD_Result ret = SD_OK;

    if (!path || !buffer || size == 0 || !bytes_read || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    *bytes_read = 0;

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_open(&file, path, FA_READ);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to open file\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OPEN;
        goto exit;
    }

    res = f_read(&file, buffer, size, &br);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to read data\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_READ;
    }
    else
    {
        *bytes_read = br;
        LOG_SD_API("Read %d bytes\r\n", br);
    }

    f_close(&file);
exit:
    xSemaphoreGive(sd_sem);
    return ret;
}

// 删除指定文件或目录
SD_Result sd_delete_file(const char *path)
{
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!path || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_unlink(path);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to delete file: %s\r\n", path);
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_DELETE;
    }
    else
    {
        LOG_SD_API("success to delete file: %s\r\n", path);
    }

    xSemaphoreGive(sd_sem);
    return ret;
}

// 获取 SD 卡剩余空间
SD_Result sd_get_free_space(uint64_t *free_bytes)
{
    FATFS *fs_ptr;
    DWORD free_clusters;
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!free_bytes || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_getfree("", &free_clusters, &fs_ptr);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to get free space\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OTHER;
    }
    else
    {
        *free_bytes = (uint64_t)free_clusters * fs_ptr->csize * 512; // 扇区大小 512 字节
        LOG_SD_API("Free space: %lld bytes\r\n", *free_bytes);
    }

    xSemaphoreGive(sd_sem);
    return ret;
}

// 列出指定目录内容
SD_Result sd_list_dir(const char *path, void (*callback)(const char *name, uint8_t is_dir, uint32_t size))
{
    DIR dir;
    FILINFO fno;
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!path || !callback || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_opendir(&dir, path);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to open directory\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OPEN;
        goto exit;
    }

    while (1)
    {
        res = f_readdir(&dir, &fno);
        if (res != FR_OK || fno.fname[0] == 0)
        {
            LOG_SD_API("Read Failed or End of directory\r\n");
            LOG_SD_API("Error code: %d\r\n", res);
            break; // 读取失败或结束
        }
        callback(fno.fname, (fno.fattrib & AM_DIR) ? 1 : 0, fno.fsize);
    }

    f_closedir(&dir);
exit:
    xSemaphoreGive(sd_sem);
    return ret;
}

// 格式化 SD 卡为 FAT32
SD_Result sd_format_card(void)
{
    uint8_t work_buffer[4096]; // 工作缓冲区
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!is_initialized)
    {
        LOG_SD_API("SD card not initialized\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_mkfs("", FM_FAT32, 0, work_buffer, sizeof(work_buffer));
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to format card\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OTHER;
    }
    else
    {
        LOG_SD_API("success to format card\r\n");
    }

    xSemaphoreGive(sd_sem);
    return ret;
}

// 重命名或移动文件/目录
SD_Result sd_rename_file(const char *old_path, const char *new_path)
{
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!old_path || !new_path || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_rename(old_path, new_path);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to rename file\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OTHER;
    }
    else
    {
        LOG_SD_API("success to rename file: %s -> %s\r\n", old_path, new_path);
    }

    xSemaphoreGive(sd_sem);
    return ret;
}

// 复制文件到新路径
SD_Result sd_copy_file(const char *src_path, const char *dst_path)
{
    FIL src_file, dst_file;
    FRESULT res;
    SD_Result ret = SD_OK;
    UINT br, bw;
    uint8_t buffer[COPY_BUFFER_SIZE];

    if (!src_path || !dst_path || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    // 打开源文件
    res = f_open(&src_file, src_path, FA_READ);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to open source file\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OPEN;
        goto exit;
    }

    // 创建目标文件
    res = f_open(&dst_file, dst_path, FA_CREATE_ALWAYS | FA_WRITE);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to create destination file\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        f_close(&src_file);
        ret = SD_ERR_OPEN;
        goto exit;
    }

    // 逐块复制
    while (1) 
    {
        res = f_read(&src_file, buffer, COPY_BUFFER_SIZE, &br);
        if (res != FR_OK || br == 0) 
        {
            LOG_SD_API("Read Failed or End of file\r\n");
            LOG_SD_API("Error code: %d\r\n", res);
            break; // 读取失败或结束
        }

        res = f_write(&dst_file, buffer, br, &bw);
        if (res != FR_OK || bw != br) 
        {
            LOG_SD_API("Write Failed\r\n");
            LOG_SD_API("Error code: %d\r\n", res);
            ret = SD_ERR_WRITE;
            break;
        }
    }

    if (res != FR_OK && ret == SD_OK)
    {
        LOG_SD_API("Failed to copy file\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_READ;
    }
    else
    {
        LOG_SD_API("success to copy file: %s -> %s\r\n", src_path, dst_path);
    }

    f_close(&src_file);
    f_close(&dst_file);
exit:
    xSemaphoreGive(sd_sem);
    return ret;
}

// 获取文件信息
SD_Result sd_get_file_info(const char *path, SD_FileInfo *info)
{
    FILINFO fno;
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!path || !info || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_stat(path, &fno);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to get file info\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OPEN;
    } 
    else 
    {
        info->size = fno.fsize;
        info->create_date = fno.fdate;
        info->create_time = fno.ftime;
        info->modify_date = fno.fdate;
        info->modify_time = fno.ftime;
        info->attrib = fno.fattrib;
        LOG_SD_API("success to get file info: %s\r\n", path);
    }

    xSemaphoreGive(sd_sem);
    return ret;
}

// 追加数据到文件
SD_Result sd_append_file(const char *path, const uint8_t *data, uint32_t size)
{
    return sd_write_file(path, data, size, 1); // 使用追加模式
}

// 截断文件到指定大小
SD_Result sd_truncate_file(const char *path, uint32_t size)
{
    FIL file;
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!path || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_open(&file, path, FA_WRITE | FA_OPEN_EXISTING);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to open file\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OPEN;
        goto exit;
    }

    res = f_truncate(&file);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to truncate file\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OTHER;
        goto exit;
    }

    if (size > f_size(&file))
    {
        res = f_lseek(&file, size);
        if (res != FR_OK)
        {
            LOG_SD_API("Failed to seek file\r\n");
            LOG_SD_API("Error code: %d\r\n", res);
            ret = SD_ERR_OTHER;
            goto exit;
        }
        res = f_truncate(&file);
        if (res != FR_OK)
        {
            LOG_SD_API("Failed to truncate file\r\n");
            LOG_SD_API("Error code: %d\r\n", res);
            ret = SD_ERR_OTHER;
        }
    }

exit:
    f_close(&file);
    xSemaphoreGive(sd_sem);
    if(ret == SD_OK)
    {
        LOG_SD_API("success to truncate file: %s\r\n", path);
    }

    return ret;
}

// 设置文件属性
SD_Result sd_set_file_attrib(const char *path, uint8_t attrib)
{
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!path || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_chmod(path, attrib, attrib);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to set file attribute\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OTHER;
    }
    else
    {
        LOG_SD_API("success to set file attribute: %s\r\n", path);
    }

    xSemaphoreGive(sd_sem);
    return ret;
}

// 获取卷信息
SD_Result sd_get_volume_info(SD_VolumeInfo *info)
{
    FATFS *fs_ptr;
    DWORD free_clusters, total_sectors;
    FRESULT res;
    SD_Result ret = SD_OK;

    if (!info || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    if (xSemaphoreTake(sd_sem, WAIT_GIVESEMAPHORE_TIMEOUT) != pdTRUE)
    {
        LOG_SD_API("Failed to take semaphore\r\n");
        return SD_ERR_OTHER;
    }

    res = f_getfree("", &free_clusters, &fs_ptr);
    if (res != FR_OK)
    {
        LOG_SD_API("Failed to get free space\r\n");
        LOG_SD_API("Error code: %d\r\n", res);
        ret = SD_ERR_OTHER;
        goto exit;
    }

    total_sectors = (fs_ptr->n_fatent - 2) * fs_ptr->csize;
    info->total_bytes = (uint64_t)total_sectors * 512;
    info->free_bytes = (uint64_t)free_clusters * fs_ptr->csize * 512;

    // 推测卡类型（基于容量）
    if (info->total_bytes <= 2ULL * 1024 * 1024 * 1024)
    {
        info->card_type = 1; // SDSC (<=2GB)
    } 
    else if (info->total_bytes <= 32ULL * 1024 * 1024 * 1024)
    {
        info->card_type = 2; // SDHC (<=32GB)
    } 
    else 
    {
        info->card_type = 3; // SDXC (>32GB)
    }

exit:
    xSemaphoreGive(sd_sem);
    if(ret == SD_OK)
    {
        LOG_SD_API("success to get volume info\r\n");
    }
    return ret;
}

// 创建或覆盖 .txt 文件
SD_Result sd_create_txt_file(const char *path, const char *initial_content)
{
    if (!path || !initial_content || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    size_t len = strlen(initial_content);
    char *buffer = pvPortMalloc(len + 2); // 分配空间，包含换行符
    if (!buffer) 
    {
        LOG_SD_API("Failed to allocate memory\r\n");
        return SD_ERR_OTHER;
    }

    memcpy(buffer, initial_content, len);
    buffer[len++] = '\n'; // 添加换行符
    buffer[len++] = '\0';

    return sd_write_file(path, (const uint8_t *)buffer, len, 0); // 覆盖模式
}

// 追加文本到 .txt 文件
SD_Result sd_append_txt_file(const char *path, const char *content)
{
    if (!path || !content || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    size_t len = strlen(content);
    char *buffer = pvPortMalloc(len + 2); // 分配空间，包含换行符
    if (!buffer) 
    {
        LOG_SD_API("Failed to allocate memory\r\n");
        return SD_ERR_OTHER;
    }

    memcpy(buffer, content, len);
    buffer[len++] = '\n'; // 添加换行符
    buffer[len++] = '\0';

    SD_Result ret = sd_write_file(path, (const uint8_t *)buffer, len, 1); // 追加模式
    vPortFree(buffer);
    return ret;
}

// 读取 .txt 文件内容
SD_Result sd_read_txt_file(const char *path, uint8_t *buffer, uint32_t size, uint32_t *bytes_read)
{
    return sd_read_file(path, buffer, size, bytes_read); // 直接调用通用读取
}

// 创建或覆盖 .csv 文件（写入表头）
SD_Result sd_create_csv_file(const char *path, const char **headers, uint32_t header_count)
{
    if (!path || !headers || header_count == 0 || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    char buffer[CSV_BUFFER_SIZE];
    uint32_t offset = 0;

    // 格式化表头
    for (uint32_t i = 0; i < header_count; i++)
    {
        if (!headers[i])
        {
            LOG_SD_API("Invalid parameter\r\n");
            return SD_ERR_INVALID_PARAM;
        }
        size_t len = strlen(headers[i]);
        if (offset + len + 3 >= CSV_BUFFER_SIZE)// 考虑引号和逗号
        {
            LOG_SD_API("Failed to create csv file: no space\r\n");
            return SD_ERR_NO_SPACE;
        }

        // 添加引号（如果字段包含逗号）
        if (strchr(headers[i], ','))
        {
            buffer[offset++] = '"';
            memcpy(buffer + offset, headers[i], len);
            offset += len;
            buffer[offset++] = '"';
        }
        else
        {
            memcpy(buffer + offset, headers[i], len);
            offset += len;
        }

        if (i < header_count - 1)
        {
            buffer[offset++] = ',';
        }
    }
    buffer[offset++] = '\n';
    buffer[offset++] = '\0';

    return sd_write_file(path, (const uint8_t *)buffer, offset, 0); // 覆盖模式
}

// 追加一行到 .csv 文件
SD_Result sd_append_csv_row(const char *path, const char **fields, uint32_t field_count)
{
    if (!path || !fields || field_count == 0 || !is_initialized)
    {
        LOG_SD_API("Invalid parameter\r\n");
        return SD_ERR_INVALID_PARAM;
    }

    char buffer[CSV_BUFFER_SIZE];
    uint32_t offset = 0;

    // 格式化一行
    for (uint32_t i = 0; i < field_count; i++)
    {
        if (!fields[i])
        {
            LOG_SD_API("Invalid parameter\r\n");
            return SD_ERR_INVALID_PARAM;
        }
        size_t len = strlen(fields[i]);
        if (offset + len + 3 >= CSV_BUFFER_SIZE) {
            return SD_ERR_NO_SPACE;
        }

        // 添加引号（如果字段包含逗号）
        if (strchr(fields[i], ','))
        {
            buffer[offset++] = '"';
            memcpy(buffer + offset, fields[i], len);
            offset += len;
            buffer[offset++] = '"';
        }
        else
        {
            memcpy(buffer + offset, fields[i], len);
            offset += len;
        }

        if (i < field_count - 1)
        {
            buffer[offset++] = ',';
        }
    }
    buffer[offset++] = '\n';
    buffer[offset++] = '\0';

    return sd_write_file(path, (const uint8_t *)buffer, offset, 1); // 追加模式
}

// 读取 .csv 文件内容
SD_Result sd_read_csv_file(const char *path, uint8_t *buffer, uint32_t size, uint32_t *bytes_read)
{
    return sd_read_file(path, buffer, size, bytes_read); // 直接调用通用读取
}
