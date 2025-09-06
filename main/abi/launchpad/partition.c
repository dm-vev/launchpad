/* -------------------------------------------------------------
 * launchpad_partition_api.c
 *
 * Implements the wrappers declared in launchpad_partition_api.h.
 * All functions return 0 on success, non‑zero otherwise,
 * matching the style of your existing SPI‑flash helpers.
 * ------------------------------------------------------------- */

#include "include/partition.h"
#include <esp_log.h>

/* ------------------------------------------------------------------
 * Utility: convert an esp_err_t into 0 / non‑zero
 * ------------------------------------------------------------------ */
static inline int _esp_status_to_int(esp_err_t err)
{
    return (err == ESP_OK) ? 0 : 1;
}

static const char *TAG = "LaunchpadPartitionAPI";

/* ------------------------------------------------------------------
 * Partition helpers
 * ------------------------------------------------------------------ */

int launchpad_partition_find(esp_partition_type_t type,
                             esp_partition_subtype_t subtype,
                             const char *label,
                             const esp_partition_t **out_part)
{
    if (!out_part) {
        ESP_LOGE(TAG, "Null out_part pointer");
        return 1;
    }

    const esp_partition_t *p = esp_partition_find_first(type, subtype, label);
    if (!p) {
        ESP_LOGE(TAG,
                 "Partition not found: type=%d, subtype=0x%x, label='%s'",
                 type, subtype, label ? label : "(NULL)");
        return 1;
    }

    *out_part = p;
    return 0;
}

int launchpad_partition_read(const esp_partition_t *partition,
                             size_t src_offset,
                             void *dst,
                             size_t size)
{
    if (!partition) {
        ESP_LOGE(TAG, "Null partition descriptor");
        return 1;
    }
    if (!dst) {
        ESP_LOGE(TAG, "Null destination buffer");
        return 1;
    }

    esp_err_t err = esp_partition_read(partition, src_offset, dst, size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
                 "Read failed: partition=%p, offset=0x%zx, size=%zu -> 0x%x",
                 partition, src_offset, size, err);
    }
    return _esp_status_to_int(err);
}

int launchpad_partition_write(const esp_partition_t *partition,
                              size_t dst_offset,
                              const void *src,
                              size_t size)
{
    if (!partition) {
        ESP_LOGE(TAG, "Null partition descriptor");
        return 1;
    }
    if (!src) {
        ESP_LOGE(TAG, "Null source buffer");
        return 1;
    }

    esp_err_t err = esp_partition_write(partition, dst_offset, src, size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
                 "Write failed: partition=%p, offset=0x%zx, size=%zu -> 0x%x",
                 partition, dst_offset, size, err);
    }
    return _esp_status_to_int(err);
}

int launchpad_partition_erase_range(const esp_partition_t *partition,
                                    uint32_t start_addr,
                                    uint32_t size)
{
    if (!partition) {
        ESP_LOGE(TAG, "Null partition descriptor");
        return 1;
    }

    esp_err_t err = esp_partition_erase_range(partition, start_addr, size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
                 "Erase range failed: partition=%p, start=0x%x, size=%u -> 0x%x",
                 partition, start_addr, size, err);
    }
    return _esp_status_to_int(err);
}

int launchpad_partition_mmap(const esp_partition_t *partition,
                             uint32_t offset,
                             uint32_t size,
                             spi_flash_mmap_memory_t memory,
                             const void **out_ptr,
                             spi_flash_mmap_handle_t *out_handle)
{
    if (!partition) {
        ESP_LOGE(TAG, "Null partition descriptor");
        return 1;
    }
    if (!out_ptr || !out_handle) {
        ESP_LOGE(TAG, "Null output pointer(s)");
        return 1;
    }

    esp_err_t err = esp_partition_mmap(partition,
                                       offset,
                                       size,
                                       memory,
                                       out_ptr,
                                       out_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
                 "MMap failed: partition=%p, offset=0x%x, size=%u -> 0x%x",
                 partition, offset, size, err);
    }
    return _esp_status_to_int(err);
}

void launchpad_partition_munmap(spi_flash_mmap_handle_t handle)
{
    /* spi_flash_munmap() has no error path – just forward it. */
    spi_flash_munmap(handle);
}

int launchpad_partition_verify(const esp_partition_t *partition)
{
    if (!partition) {
        ESP_LOGE(TAG, "Null partition descriptor");
        return 1;
    }

    const esp_partition_t *verified = esp_partition_verify(partition);
    if (verified != partition) {
        ESP_LOGE(TAG,
                 "Partition record not found in table: %p vs %p",
                 partition, verified);
        return 1;
    }
    return 0;
}

bool launchpad_partition_is_encrypted(void)
{
#if CONFIG_FLASH_ENCRYPTION
    return esp_flash_encryption_enabled();
#else
    /* If encryption support was disabled at compile time,
     * it cannot be enabled in runtime. */
    return false;
#endif
}
