/* -------------------------------------------------------------
 * launchpad_flash_api.h
 *
 * Public wrapper around the ESP-IDF SPI‑flash functions.
 * The API follows the same “0 = success, non‑zero = error” style
 * that you already use in your code base.
 * ------------------------------------------------------------- */

#ifndef LAUNCHPAD_FLASH_API_H
#define LAUNCHPAD_FLASH_API_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* Pull in the official ESP‑IDF definitions for SPI flash types/functions. */
#include "esp_spi_flash.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the size of the flash chip (in bytes).
 *
 * @return Size in bytes.
 */
size_t launchpad_flash_size(void);

/**
 * @brief Erase a single sector (4 KiB) at the given index.
 *
 * @param sector 0‑based sector number.
 * @return 0 on success, non‑zero on failure.
 */
int launchpad_flash_erase(size_t sector);

/**
 * @brief Erase a range of flash starting at an address.
 *
 * @param start_address Start address (must be 4 KiB aligned).
 * @param size Size in bytes (must be a multiple of 4 KiB).
 * @return 0 on success, non‑zero on failure.
 */
int launchpad_flash_erase_range(size_t start_address, size_t size);

/**
 * @brief Read data from flash into RAM.
 *
 * @param src_addr Source address in flash.
 * @param dest Destination buffer (must be large enough).
 * @param size Number of bytes to read.
 * @return 0 on success, non‑zero on failure.
 */
int launchpad_flash_read(size_t src_addr, void *dest, size_t size);

/**
 * @brief Write data from RAM into flash.
 *
 * @param dest_addr Destination address in flash (4‑byte aligned).
 * @param src Source buffer.
 * @param size Number of bytes to write (multiple of 4).
 * @return 0 on success, non‑zero on failure.
 */
int launchpad_flash_write(size_t dest_addr,
                          const void *src,
                          size_t size);

/**
 * @brief Write encrypted data into flash.
 *
 * Only available when Flash encryption is enabled in the device
 * (CONFIG_FLASH_ENCRYPTION).  The address and length must be
 * multiples of 16 bytes.
 *
 * @param dest_addr Destination address in flash.
 * @param src Source buffer.
 * @param size Number of bytes to write.
 * @return 0 on success, non‑zero on failure.
 */
int launchpad_flash_write_encrypted(size_t dest_addr,
                                    const void *src,
                                    size_t size);

/**
 * @brief Map a region of flash into the CPU address space.
 *
 * The mapping is read‑only.  After use call `launchpad_flash_munmap()`.
 *
 * @param src_addr Physical address in flash (must be 64 KiB aligned).
 * @param size Size to map (rounded up to 64 KiB).
 * @param memory Either SPI_FLASH_MMAP_DATA or SPI_FLASH_MMAP_INST.
 * @param out_ptr Pointer that will receive the mapped virtual address.
 * @param out_handle Handle that must be passed to munmap().
 *
 * @return 0 on success, non‑zero on failure.
 */
int launchpad_flash_mmap(size_t src_addr,
                         size_t size,
                         spi_flash_mmap_memory_t memory,
                         const void **out_ptr,
                         spi_flash_mmap_handle_t *out_handle);

/**
 * @brief Unmap a previously mapped flash region.
 *
 * @param handle Handle obtained from `launchpad_flash_mmap()`.
 */
void launchpad_flash_munmap(spi_flash_mmap_handle_t handle);

/**
 * @brief Check whether Flash encryption is active.
 *
 * @return true if enabled, false otherwise.
 */
bool launchpad_flash_is_encrypted(void);

#ifdef __cplusplus
}
#endif

#endif /* LAUNCHPAD_FLASH_API_H */
