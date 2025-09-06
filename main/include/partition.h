/* -------------------------------------------------------------
 * launchpad_partition_api.h
 *
 * Lightweight wrappers around the ESP‑IDF partition APIs.
 * All functions return **0 on success** and a non‑zero value on error,
 * mirroring the style you already use for SPI‑flash operations.
 *
 * The header can be dropped into <your‑project>/include/
 * ------------------------------------------------------------- */

#ifndef LAUNCHPAD_PARTITION_API_H
#define LAUNCHPAD_PARTITION_API_H

#include <stddef.h>
#include <stdbool.h>

#include "esp_partition.h"
#include "esp_spi_flash.h"          /* for spi_flash_mmap_handle_t */
#include "esp_flash_encrypt.h"

/* ------------------------------------------------------------------
 *  Mapping memory type – re‑export the enum from esp_spi_flash.h
 * ------------------------------------------------------------------ */
typedef enum {
    SPI_FLASH_MMAP_DATA = 0,
    SPI_FLASH_MMAP_INST = 1
} spi_flash_mmap_memory_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Find a partition by type/subtype/label.
 *
 * This is a thin wrapper around esp_partition_find_first().
 *
 * @param type     Partition type (e.g. ESP_PARTITION_TYPE_APP).
 * @param subtype  Sub‑type or ESP_PARTITION_SUBTYPE_ANY.
 * @param label    Optional label string; pass NULL to ignore.
 * @param out_part Pointer that will receive the partition descriptor.
 * @return 0 on success, non‑zero if not found or a bad argument.
 */
int launchpad_partition_find(esp_partition_type_t type,
                             esp_partition_subtype_t subtype,
                             const char *label,
                             const esp_partition_t **out_part);

/**
 * @brief Read data from a partition into RAM.
 *
 * @param partition Partition descriptor (must be non‑NULL).
 * @param src_offset Offset inside the partition.
 * @param dst        Destination buffer (must be large enough).
 * @param size       Number of bytes to read.
 * @return 0 on success, non‑zero on error.
 */
int launchpad_partition_read(const esp_partition_t *partition,
                             size_t src_offset,
                             void *dst,
                             size_t size);

/**
 * @brief Write data from RAM into a partition.
 *
 * The caller must ensure that the target region has already been erased
 * (use launchpad_partition_erase_range() first).
 *
 * @param partition Partition descriptor (must be non‑NULL).
 * @param dst_offset Offset inside the partition.
 * @param src        Source buffer.
 * @param size       Number of bytes to write.
 * @return 0 on success, non‑zero on error.
 */
int launchpad_partition_write(const esp_partition_t *partition,
                              size_t dst_offset,
                              const void *src,
                              size_t size);

/**
 * @brief Erase a range inside a partition.
 *
 * The `start_addr` and `size` arguments are expressed in **bytes** relative
 * to the beginning of the partition.  They must be 4 KiB‑aligned / multiple.
 *
 * @param partition Partition descriptor (must be non‑NULL).
 * @param start_addr Start address inside the partition (offset).
 * @param size       Size of the range to erase, in bytes.
 * @return 0 on success, non‑zero on error.
 */
int launchpad_partition_erase_range(const esp_partition_t *partition,
                                    uint32_t start_addr,
                                    uint32_t size);

/**
 * @brief Map a part of a partition into CPU address space (read‑only).
 *
 * The mapping is performed by the underlying `spi_flash_mmap()` call, so
 * it obeys the 64 KiB page granularity and cache restrictions.
 *
 * @param partition Partition descriptor (must be non‑NULL).
 * @param offset    Offset inside the partition to start mapping.
 * @param size      Size of region to map (will be rounded up to 64 KiB).
 * @param memory    Either SPI_FLASH_MMAP_DATA or SPI_FLASH_MMAP_INST.
 * @param out_ptr   Pointer that will receive the virtual address of the
 *                  mapped region.  The returned pointer may not point at
 *                  `offset` itself – it might be earlier because of page
 *                  alignment; the caller should use this pointer directly.
 * @param out_handle Handle that must later be passed to launchpad_partition_munmap().
 *
 * @return 0 on success, non‑zero on error.
 */
int launchpad_partition_mmap(const esp_partition_t *partition,
                             uint32_t offset,
                             uint32_t size,
                             spi_flash_mmap_memory_t memory,
                             const void **out_ptr,
                             spi_flash_mmap_handle_t *out_handle);

/**
 * @brief Unmap a previously mapped partition region.
 *
 * This is just a thin wrapper around `spi_flash_munmap()`.
 *
 * @param handle Handle obtained from launchpad_partition_mmap().
 */
void launchpad_partition_munmap(spi_flash_mmap_handle_t handle);

/**
 * @brief Verify that the provided descriptor really belongs to the
 *        partition table.  Useful when you have a copy of the struct in RAM.
 *
 * @param partition Pointer to a potentially user‑supplied partition record.
 * @return 0 if the record matches an entry in the partition table,
 *         non‑zero otherwise.
 */
int launchpad_partition_verify(const esp_partition_t *partition);

/**
 * @brief Return true iff Flash encryption is enabled on this device.
 *
 * The wrapper just forwards `esp_flash_encryption_enabled()`.
 *
 * @return true if encryption active, false otherwise.
 */
bool launchpad_partition_is_encrypted(void);

#ifdef __cplusplus
}
#endif

#endif /* LAUNCHPAD_PARTITION_API_H */
