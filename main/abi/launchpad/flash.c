/* -------------------------------------------------------------
 * launchpad_flash_api.c
 *
 * Implements the public wrapper API defined in launchpad_flash_api.h.
 * All functions return 0 on success, non‑zero on failure (matching
 * your existing `launchpad_flash_erase*()` style).
 * ------------------------------------------------------------- */

#include "include/flash.h"

#include <esp_log.h>
#include <bootloader_support/include/esp_flash_encrypt.h> /* esp_flash_encryption_enabled() */
#include "esp_spi_flash.h"

/* ------------------------------------------------------------------
 * Internal helper: Convert an ESP-IDF esp_err_t into 0 / non‑zero.
 * -------------------------------------------------------------- */
static inline int _esp_status_to_int(esp_err_t err)
{
    return (err == ESP_OK) ? 0 : 1;
}

static const char *TAG = "LaunchpadFlashAPI";

/* ------------------------------------------------------------------
 * Public API implementations
 * ------------------------------------------------------------------ */

size_t launchpad_flash_size(void)
{
    /* The underlying function already returns the size in bytes. */
    return spi_flash_get_chip_size();
}

/* -------------------------------------------------------------- */
int launchpad_flash_erase(size_t sector)
{
    esp_err_t status = spi_flash_erase_sector(sector);
    if (status != ESP_OK) {
        ESP_LOGE(TAG, "Erase sector %zu failed: 0x%x", sector, status);
    }
    return _esp_status_to_int(status);
}

/* -------------------------------------------------------------- */
int launchpad_flash_erase_range(size_t start_address, size_t size)
{
    esp_err_t status = spi_flash_erase_range(start_address, size);
    if (status != ESP_OK) {
        ESP_LOGE(TAG,
                 "Erase range 0x%zx..0x%zx failed: 0x%x",
                 start_address,
                 start_address + size - 1,
                 status);
    }
    return _esp_status_to_int(status);
}

/* -------------------------------------------------------------- */
int launchpad_flash_read(size_t src_addr, void *dest, size_t size)
{
    if (!dest) {
        ESP_LOGE(TAG, "Null destination buffer");
        return 1;
    }

    esp_err_t status = spi_flash_read(src_addr, dest, size);
    if (status != ESP_OK) {
        ESP_LOGE(TAG, "Read from 0x%zx failed: 0x%x", src_addr, status);
    }
    return _esp_status_to_int(status);
}

/* -------------------------------------------------------------- */
int launchpad_flash_write(size_t dest_addr,
                          const void *src,
                          size_t size)
{
    if (!src) {
        ESP_LOGE(TAG, "Null source buffer");
        return 1;
    }

    esp_err_t status = spi_flash_write(dest_addr, src, size);
    if (status != ESP_OK) {
        ESP_LOGE(TAG, "Write to 0x%zx failed: 0x%x", dest_addr, status);
    }
    return _esp_status_to_int(status);
}

/* -------------------------------------------------------------- */
int launchpad_flash_write_encrypted(size_t dest_addr,
                                    const void *src,
                                    size_t size)
{
#if CONFIG_FLASH_ENCRYPTION
    if (!esp_flash_encryption_enabled()) {
        ESP_LOGE(TAG, "Flash encryption not enabled");
        return 1;
    }
#else
    /* If encryption support is disabled at compile time we just fall back
     * to a normal write – the caller can decide whether that is OK. */
    ESP_LOGW(TAG,
             "Encryption disabled in config; falling back to plain write");
    return launchpad_flash_write(dest_addr, src, size);
#endif

    if (!src) {
        ESP_LOGE(TAG, "Null source buffer for encrypted write");
        return 1;
    }

    esp_err_t status = spi_flash_write_encrypted(dest_addr, src, size);
    if (status != ESP_OK) {
        ESP_LOGE(TAG,
                 "Encrypted write to 0x%zx failed: 0x%x",
                 dest_addr,
                 status);
    }
    return _esp_status_to_int(status);
}

/* -------------------------------------------------------------- */
int launchpad_flash_mmap(size_t src_addr,
                         size_t size,
                         spi_flash_mmap_memory_t memory,
                         const void **out_ptr,
                         spi_flash_mmap_handle_t *out_handle)
{
    if (!out_ptr || !out_handle) {
        ESP_LOGE(TAG, "Null output pointer");
        return 1;
    }

    esp_err_t status = spi_flash_mmap(src_addr,
                                      size,
                                      memory,
                                      out_ptr,
                                      out_handle);
    if (status != ESP_OK) {
        ESP_LOGE(TAG,
                 "MMap of 0x%zx..0x%zx failed: 0x%x",
                 src_addr,
                 src_addr + size - 1,
                 status);
    }
    return _esp_status_to_int(status);
}

/* -------------------------------------------------------------- */
void launchpad_flash_munmap(spi_flash_mmap_handle_t handle)
{
    /* The underlying function has no error path – just unmap. */
    spi_flash_munmap(handle);
}

/* -------------------------------------------------------------- */
bool launchpad_flash_is_encrypted(void)
{
#if CONFIG_FLASH_ENCRYPTION
    return esp_flash_encryption_enabled();
#else
    return false;
#endif
}
