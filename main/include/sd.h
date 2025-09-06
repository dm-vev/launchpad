/* launchpad_sd.h */
#ifndef LAUNCHPAD_SD_H_
#define LAUNCHPAD_SD_H_

#include <stdbool.h>
#include <stdint.h>
#include "esp_err.h"
#include "driver/gpio.h"      /* gpio_num_t, GPIO_NUM_* */
#include "sdmmc_cmd.h"        /* sdmmc_card_t, SDMMC_FREQ_* */

typedef struct {
    const char *mount_path;
    uint32_t max_files;
    bool format_if_mount_failed;
    gpio_num_t cs_pin, sclk_pin, mosi_pin, miso_pin;
    uint32_t max_freq_khz; /* 0 → SDMMC_FREQ_DEFAULT */
} launchpad_sd_config_t;

static inline launchpad_sd_config_t launchpad_sd_default_cfg(void)
{
    return (launchpad_sd_config_t){
        .mount_path            = "/sdcard",
        .max_files             = 16,
        .format_if_mount_failed= false,
        .cs_pin                = GPIO_NUM_39,
        .sclk_pin              = GPIO_NUM_40,
        .mosi_pin              = GPIO_NUM_41,
        .miso_pin              = GPIO_NUM_38,
        .max_freq_khz          = SDMMC_FREQ_DEFAULT
    };
}

/* Основные операции */
esp_err_t launchpad_sd_mount(const launchpad_sd_config_t *cfg);
esp_err_t launchpad_sd_unmount(const char *mount_path);

/* Статус и сведения */
bool launchpad_sd_available(void);
bool launchpad_sd_is_mounted(const char *mount_path);
esp_err_t launchpad_sd_get_card(sdmmc_card_t **card);

size_t launchpad_sd_get_free_space_bytes(void);

/* Низкоуровневый доступ (команды) */
esp_err_t launchpad_sd_send_cmd(uint8_t cmd, uint32_t arg,
                                sdmmc_response_t resp_type,
                                uint32_t *resp);

#endif /* LAUNCHPAD_SD_H_ */
