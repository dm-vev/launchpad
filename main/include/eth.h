/* -------------------------------------------------------------
 * launchpad_eth_api.h
 *
 * Lightweight Ethernet initialization/deinitialization API
 * for internal and SPI-based Ethernet modules on ESP32.
 *
 * ------------------------------------------------------------- */

#ifndef LAUNCHPAD_ETH_API_H
#define LAUNCHPAD_ETH_API_H

#include <stdint.h>
#include "esp_err.h"
#include "esp_eth.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize all available Ethernet interfaces.
 *
 * Depending on Kconfig (CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET /
 * CONFIG_EXAMPLE_USE_SPI_ETHERNET), this will set up internal
 * EMAC or external SPI Ethernet modules.
 *
 * @param[out] eth_handles_out Pointer to array of Ethernet handles.
 *                             Caller must free() after deinit.
 * @param[out] eth_cnt_out     Number of initialized interfaces.
 *
 * @return ESP_OK on success, error code otherwise.
 */
esp_err_t launchpad_eth_init(esp_eth_handle_t **eth_handles_out, uint8_t *eth_cnt_out);

/**
 * @brief Deinitialize Ethernet interfaces.
 *
 * @param[in] eth_handles Array of Ethernet handles returned by launchpad_eth_init().
 * @param[in] eth_cnt     Number of interfaces.
 *
 * @return ESP_OK on success, error code otherwise.
 */
esp_err_t launchpad_eth_deinit(esp_eth_handle_t *eth_handles, uint8_t eth_cnt);

#ifdef __cplusplus
}
#endif

#endif /* LAUNCHPAD_ETH_API_H */
