#include "include/eth.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
#include "driver/spi_master.h"
#endif

static const char *TAG = "launchpad_eth";

#if CONFIG_EXAMPLE_SPI_ETHERNETS_NUM
#define SPI_ETHERNETS_NUM           CONFIG_EXAMPLE_SPI_ETHERNETS_NUM
#else
#define SPI_ETHERNETS_NUM           0
#endif

#if CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET
#define INTERNAL_ETHERNETS_NUM      1
#else
#define INTERNAL_ETHERNETS_NUM      0
#endif

#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
static bool gpio_isr_svc_init_by_eth = false;
#endif

/* ---------------- Internal Ethernet ---------------- */
#if CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET
static esp_eth_handle_t eth_init_internal(esp_eth_mac_t **mac_out, esp_eth_phy_t **phy_out)
{
    esp_eth_handle_t ret = NULL;

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    phy_config.phy_addr = CONFIG_EXAMPLE_ETH_PHY_ADDR;
    phy_config.reset_gpio_num = CONFIG_EXAMPLE_ETH_PHY_RST_GPIO;

    eth_esp32_emac_config_t esp32_emac_config = ETH_ESP32_EMAC_DEFAULT_CONFIG();
    esp32_emac_config.smi_gpio.mdc_num = CONFIG_EXAMPLE_ETH_MDC_GPIO;
    esp32_emac_config.smi_gpio.mdio_num = CONFIG_EXAMPLE_ETH_MDIO_GPIO;

#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
    esp32_emac_config.dma_burst_len = ETH_DMA_BURST_LEN_4;
#endif

    esp_eth_mac_t *mac = esp_eth_mac_new_esp32(&esp32_emac_config, &mac_config);

#if CONFIG_EXAMPLE_ETH_PHY_GENERIC
    esp_eth_phy_t *phy = esp_eth_phy_new_generic(&phy_config);
#elif CONFIG_EXAMPLE_ETH_PHY_IP101
    esp_eth_phy_t *phy = esp_eth_phy_new_ip101(&phy_config);
#elif CONFIG_EXAMPLE_ETH_PHY_RTL8201
    esp_eth_phy_t *phy = esp_eth_phy_new_rtl8201(&phy_config);
#elif CONFIG_EXAMPLE_ETH_PHY_LAN87XX
    esp_eth_phy_t *phy = esp_eth_phy_new_lan87xx(&phy_config);
#elif CONFIG_EXAMPLE_ETH_PHY_DP83848
    esp_eth_phy_t *phy = esp_eth_phy_new_dp83848(&phy_config);
#elif CONFIG_EXAMPLE_ETH_PHY_KSZ80XX
    esp_eth_phy_t *phy = esp_eth_phy_new_ksz80xx(&phy_config);
#endif

    esp_eth_handle_t eth_handle = NULL;
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    ESP_GOTO_ON_FALSE(esp_eth_driver_install(&config, &eth_handle) == ESP_OK, NULL,
                        err, TAG, "Ethernet driver install failed");

    if (mac_out) *mac_out = mac;
    if (phy_out) *phy_out = phy;
    return eth_handle;

err:
    if (eth_handle) esp_eth_driver_uninstall(eth_handle);
    if (mac) mac->del(mac);
    if (phy) phy->del(phy);
    return ret;
}
#endif // CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET

/* ---------------- SPI Ethernet ---------------- */
#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
typedef struct {
    uint8_t spi_cs_gpio;
    int8_t int_gpio;
    uint32_t polling_ms;
    int8_t phy_reset_gpio;
    uint8_t phy_addr;
    uint8_t *mac_addr;
} spi_eth_module_config_t;

#define INIT_SPI_ETH_MODULE_CONFIG(eth_module_config, num)                                      \
    do {                                                                                        \
        eth_module_config[num].spi_cs_gpio = CONFIG_EXAMPLE_ETH_SPI_CS ##num## _GPIO;           \
        eth_module_config[num].int_gpio = CONFIG_EXAMPLE_ETH_SPI_INT ##num## _GPIO;             \
        eth_module_config[num].polling_ms = CONFIG_EXAMPLE_ETH_SPI_POLLING ##num## _MS;         \
        eth_module_config[num].phy_reset_gpio = CONFIG_EXAMPLE_ETH_SPI_PHY_RST ##num## _GPIO;   \
        eth_module_config[num].phy_addr = CONFIG_EXAMPLE_ETH_SPI_PHY_ADDR ##num;                \
    } while(0)

static esp_err_t spi_bus_init(void)
{
    esp_err_t ret = ESP_OK;
#if (CONFIG_EXAMPLE_ETH_SPI_INT0_GPIO >= 0) || (CONFIG_EXAMPLE_ETH_SPI_INT1_GPIO > 0)
    ret = gpio_install_isr_service(0);
    if (ret == ESP_OK) {
        gpio_isr_svc_init_by_eth = true;
    } else if (ret == ESP_ERR_INVALID_STATE) {
        ESP_LOGW(TAG, "GPIO ISR handler already installed");
        ret = ESP_OK;
    } else {
        ESP_LOGE(TAG, "GPIO ISR handler install failed");
        return ret;
    }
#endif

    spi_bus_config_t buscfg = {
        .miso_io_num = CONFIG_EXAMPLE_ETH_SPI_MISO_GPIO,
        .mosi_io_num = CONFIG_EXAMPLE_ETH_SPI_MOSI_GPIO,
        .sclk_io_num = CONFIG_EXAMPLE_ETH_SPI_SCLK_GPIO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(CONFIG_EXAMPLE_ETH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO), TAG,
                        "SPI host #%d init failed", CONFIG_EXAMPLE_ETH_SPI_HOST);
    return ret;
}

static esp_eth_handle_t eth_init_spi(spi_eth_module_config_t *spi_eth_module_config,
                                     esp_eth_mac_t **mac_out, esp_eth_phy_t **phy_out)
{
    esp_eth_handle_t ret = NULL;

    eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
    eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();

    phy_config.phy_addr = spi_eth_module_config->phy_addr;
    phy_config.reset_gpio_num = spi_eth_module_config->phy_reset_gpio;

    spi_device_interface_config_t spi_devcfg = {
        .mode = 0,
        .clock_speed_hz = CONFIG_EXAMPLE_ETH_SPI_CLOCK_MHZ * 1000 * 1000,
        .queue_size = 20,
        .spics_io_num = spi_eth_module_config->spi_cs_gpio
    };

#if CONFIG_EXAMPLE_USE_KSZ8851SNL
    eth_ksz8851snl_config_t ksz_cfg = ETH_KSZ8851SNL_DEFAULT_CONFIG(CONFIG_EXAMPLE_ETH_SPI_HOST, &spi_devcfg);
    ksz_cfg.int_gpio_num = spi_eth_module_config->int_gpio;
    ksz_cfg.poll_period_ms = spi_eth_module_config->polling_ms;
    esp_eth_mac_t *mac = esp_eth_mac_new_ksz8851snl(&ksz_cfg, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_ksz8851snl(&phy_config);
#elif CONFIG_EXAMPLE_USE_DM9051
    eth_dm9051_config_t dm_cfg = ETH_DM9051_DEFAULT_CONFIG(CONFIG_EXAMPLE_ETH_SPI_HOST, &spi_devcfg);
    dm_cfg.int_gpio_num = spi_eth_module_config->int_gpio;
    dm_cfg.poll_period_ms = spi_eth_module_config->polling_ms;
    esp_eth_mac_t *mac = esp_eth_mac_new_dm9051(&dm_cfg, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_dm9051(&phy_config);
#elif CONFIG_EXAMPLE_USE_W5500
    eth_w5500_config_t w5500_cfg = ETH_W5500_DEFAULT_CONFIG(CONFIG_EXAMPLE_ETH_SPI_HOST, &spi_devcfg);
    w5500_cfg.int_gpio_num = spi_eth_module_config->int_gpio;
    w5500_cfg.poll_period_ms = spi_eth_module_config->polling_ms;
    esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_cfg, &mac_config);
    esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);
#endif

    esp_eth_handle_t eth_handle = NULL;
    esp_eth_config_t config = ETH_DEFAULT_CONFIG(mac, phy);
    ESP_GOTO_ON_FALSE(esp_eth_driver_install(&config, &eth_handle) == ESP_OK, NULL,
                      err, TAG, "SPI Ethernet driver install failed");

    if (spi_eth_module_config->mac_addr) {
        ESP_GOTO_ON_FALSE(esp_eth_ioctl(eth_handle, ETH_CMD_S_MAC_ADDR, spi_eth_module_config->mac_addr) == ESP_OK,
                          NULL, err, TAG, "SPI Ethernet MAC config failed");
    }

    if (mac_out) *mac_out = mac;
    if (phy_out) *phy_out = phy;
    return eth_handle;
err:
    if (eth_handle) esp_eth_driver_uninstall(eth_handle);
    if (mac) mac->del(mac);
    if (phy) phy->del(phy);
    return ret;
}
#endif // CONFIG_EXAMPLE_USE_SPI_ETHERNET

/* ---------------- Public API ---------------- */

esp_err_t launchpad_eth_init(esp_eth_handle_t **eth_handles_out, uint8_t *eth_cnt_out)
{
    esp_err_t ret = ESP_OK;
    esp_eth_handle_t *eth_handles = NULL;
    uint8_t eth_cnt = 0;

#if CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET || CONFIG_EXAMPLE_USE_SPI_ETHERNET
    ESP_GOTO_ON_FALSE(eth_handles_out && eth_cnt_out, ESP_ERR_INVALID_ARG, err, TAG, "invalid args");
    eth_handles = calloc(SPI_ETHERNETS_NUM + INTERNAL_ETHERNETS_NUM, sizeof(esp_eth_handle_t));
    ESP_GOTO_ON_FALSE(eth_handles, ESP_ERR_NO_MEM, err, TAG, "no memory");

#if CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET
    eth_handles[eth_cnt] = eth_init_internal(NULL, NULL);
    ESP_GOTO_ON_FALSE(eth_handles[eth_cnt], ESP_FAIL, err, TAG, "internal eth init failed");
    eth_cnt++;
#endif

#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
    ESP_GOTO_ON_ERROR(spi_bus_init(), err, TAG, "SPI bus init failed");

    spi_eth_module_config_t spi_eth_module_config[SPI_ETHERNETS_NUM] = { 0 };
    INIT_SPI_ETH_MODULE_CONFIG(spi_eth_module_config, 0);

    uint8_t base_mac[ETH_ADDR_LEN];
    ESP_GOTO_ON_ERROR(esp_efuse_mac_get_default(base_mac), err, TAG, "get EFUSE MAC failed");

    uint8_t local_mac_1[ETH_ADDR_LEN];
    esp_derive_local_mac(local_mac_1, base_mac);
    spi_eth_module_config[0].mac_addr = local_mac_1;

#if SPI_ETHERNETS_NUM > 1
    INIT_SPI_ETH_MODULE_CONFIG(spi_eth_module_config, 1);
    uint8_t local_mac_2[ETH_ADDR_LEN];
    base_mac[ETH_ADDR_LEN - 1] += 1;
    esp_derive_local_mac(local_mac_2, base_mac);
    spi_eth_module_config[1].mac_addr = local_mac_2;
#endif

#if SPI_ETHERNETS_NUM > 2
#error Max 2 SPI Ethernet devices supported
#endif

    for (int i = 0; i < SPI_ETHERNETS_NUM; i++) {
        eth_handles[eth_cnt] = eth_init_spi(&spi_eth_module_config[i], NULL, NULL);
        ESP_GOTO_ON_FALSE(eth_handles[eth_cnt], ESP_FAIL, err, TAG, "SPI eth init failed");
        eth_cnt++;
    }
#endif
#else
    ESP_LOGD(TAG, "no Ethernet selected in config");
#endif

    *eth_handles_out = eth_handles;
    *eth_cnt_out = eth_cnt;
    return ret;

#if CONFIG_EXAMPLE_USE_INTERNAL_ETHERNET || CONFIG_EXAMPLE_USE_SPI_ETHERNET
err:
    free(eth_handles);
    return ret;
#endif
}

esp_err_t launchpad_eth_deinit(esp_eth_handle_t *eth_handles, uint8_t eth_cnt)
{
    ESP_RETURN_ON_FALSE(eth_handles, ESP_ERR_INVALID_ARG, TAG, "handles cannot be NULL");
    for (int i = 0; i < eth_cnt; i++) {
        esp_eth_mac_t *mac = NULL;
        esp_eth_phy_t *phy = NULL;
        if (eth_handles[i]) {
            esp_eth_get_mac_instance(eth_handles[i], &mac);
            esp_eth_get_phy_instance(eth_handles[i], &phy);
            ESP_RETURN_ON_ERROR(esp_eth_driver_uninstall(eth_handles[i]), TAG, "eth uninstall failed");
        }
        if (mac) mac->del(mac);
        if (phy) phy->del(phy);
    }
#if CONFIG_EXAMPLE_USE_SPI_ETHERNET
    spi_bus_free(CONFIG_EXAMPLE_ETH_SPI_HOST);
#if (CONFIG_EXAMPLE_ETH_SPI_INT0_GPIO >= 0) || (CONFIG_EXAMPLE_ETH_SPI_INT1_GPIO > 0)
    if (gpio_isr_svc_init_by_eth) {
        ESP_LOGW(TAG, "uninstalling GPIO ISR service!");
        gpio_uninstall_isr_service();
    }
#endif
#endif
    free(eth_handles);
    return ESP_OK;
}
