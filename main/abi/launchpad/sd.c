/* launchpad_sd.c */

#include "include/sd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <driver/gpio.h>
#include <driver/spi_common.h>
#include <esp_vfs_fat.h>
#include <esp_log.h>
#include <platform.h>

#define TAG "LaunchPadSD"

static sdmmc_card_t *s_card = NULL;
static bool s_mounted = false;
static spi_host_device_t s_host = SPI2_HOST; // default host
static const char *s_mount_path = NULL;

/* Внутренний вспомогательный «приватный» вызов */
static esp_err_t _sd_mount(const launchpad_sd_config_t *cfg)
{
    if (s_mounted) {
        ESP_LOGW(TAG, "SD already mounted");
        return ESP_ERR_INVALID_STATE;
    }

    /* 1. Настраиваем SPI‑шину (один раз – один bus) */
    spi_bus_config_t bus_cfg = {0};
    bus_cfg.mosi_io_num   = cfg->mosi_pin;
    bus_cfg.miso_io_num   = cfg->miso_pin;
    bus_cfg.sclk_io_num   = cfg->sclk_pin;
    bus_cfg.quadwp_io_num = -1;
    bus_cfg.quadhd_io_num = -1;
    bus_cfg.max_transfer_sz = 4000;

    esp_err_t err = spi_bus_initialize(s_host, &bus_cfg, SDSPI_DEFAULT_DMA);
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "spi_bus_initialize error: %s", esp_err_to_name(err));
        return err;
    }

    /* 2. Настраиваем CS‑пин */
    gpio_config_t io_cfg = {0};
    io_cfg.pin_bit_mask = BIT64(cfg->cs_pin);
    io_cfg.mode          = GPIO_MODE_OUTPUT;
    io_cfg.pull_up_en    = GPIO_PULLUP_DISABLE;
    io_cfg.pull_down_en  = GPIO_PULLDOWN_DISABLE;
    io_cfg.intr_type     = GPIO_INTR_DISABLE;
    gpio_config(&io_cfg);
    gpio_set_level(cfg->cs_pin, 1);                 // CS idle high

    /* 3. Создаём конфиг устройства */
    sdspi_device_config_t slot_cfg = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_cfg.gpio_cs   = cfg->cs_pin;
    slot_cfg.host_id   = s_host;

    /* 4. Конфигурация VFS‑FAT */
    esp_vfs_fat_mount_config_t mount_cfg = {
        .format_if_mount_failed   = cfg->format_if_mount_failed,
        .max_files                = cfg->max_files,
        .allocation_unit_size     = 16 * 1024,
        .disk_status_check_enable = false,
        .use_one_fat              = false
    };

    /* 5. Настраиваем хост */
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot           = s_host;
    host.max_freq_khz   = cfg->max_freq_khz;

    /* 6. Монтируем! */
    err = esp_vfs_fat_sdspi_mount(cfg->mount_path, &host, &slot_cfg,
                                  &mount_cfg, &s_card);
    if (err == ESP_OK) {
        s_mounted      = true;
        s_mount_path   = cfg->mount_path;
    } else {
        ESP_LOGE(TAG, "SD mount failed: %s", esp_err_to_name(err));
    }
    return err;
}

esp_err_t launchpad_sd_mount(const launchpad_sd_config_t *cfg)
{
    if (!launchpad_sd_available()) {
        ESP_LOGE(TAG, "SD feature not available on this platform");
        return ESP_ERR_NOT_SUPPORTED;
    }
    return _sd_mount(cfg);
}

esp_err_t launchpad_sd_unmount(const char *mount_path)
{
    if (!s_mounted) {
        ESP_LOGW(TAG, "No SD card mounted");
        return ESP_ERR_INVALID_STATE;
    }

    /* Проверяем, совпадает ли путь с тем, что был использован при монтировании */
    if (strcmp(mount_path, s_mount_path) != 0) {
        ESP_LOGW(TAG, "Unmount path mismatch: requested=%s actual=%s",
                 mount_path, s_mount_path);
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t err = esp_vfs_fat_sdmmc_unmount(mount_path, s_card);
    if (err == ESP_OK) {
        s_mounted  = false;
        s_card     = NULL;
        s_mount_path = NULL;
    } else {
        ESP_LOGE(TAG, "SD unmount failed: %s", esp_err_to_name(err));
    }
    return err;
}

bool launchpad_sd_available(void)
{
    /* Проверяем флаг в структуре платформы */
    return (launchpad_platform().features & LAUNCHPAD_FEATURE_SD_CARD) != 0;
}

bool launchpad_sd_is_mounted(const char *mount_path)
{
    if (!s_mounted) return false;
    return strcmp(mount_path, s_mount_path) == 0;
}

esp_err_t launchpad_sd_get_card(sdmmc_card_t **card)
{
    if (!s_mounted || !card) {
        return ESP_ERR_INVALID_STATE;
    }
    *card = s_card;
    return ESP_OK;
}

/* Возвращает количество свободных байт (если возможно), иначе 0 */
size_t launchpad_sd_get_free_space_bytes(void)
{
    if (!s_mounted) return 0;

    uint64_t total = 0, used = 0;
    esp_err_t err = esp_vfs_fat_info(s_mount_path, &total, &used);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get free space: %s", esp_err_to_name(err));
        return 0;
    }
    return total - used;
}

/* Низкоуровневый доступ: отправка произвольной команды SD */
esp_err_t launchpad_sd_send_cmd(uint8_t cmd, uint32_t arg,
                                sdmmc_response_type_t resp_type,
                                sdmmc_response_t *resp)
{
    if (!s_mounted) return ESP_ERR_INVALID_STATE;

    /* Используем helper из sdmmc_cmd.h */
    return sdmmc_app_command(s_host, s_card, cmd, arg, resp_type, resp);
}
