#include "platform.h"

static const launchpad_platform_info_t g_launchpad_info = {
    .magic          = LAUNCHPAD_MAGIC,

    /* Версия загрузчика: 1.0.3 */
    .version_major  = 1,
    .version_minor  = 0,
    .version_patch  = 3,
    .build_timestamp= 0x20250823U,

    /* Строки: обязательно завершаем NUL, остальные байты могут быть пустыми */
    .loader_name     = "LaunchPad",
    .build_name      = "Developer Build Preview",
    .platform_name   = "ESP32",

    .bitness         = 32,
    .endian          = LAUNCHPAD_ENDIAN_LITTLE,

    .reserved0       = 0,                /* выравнивание */

    .features        =         LAUNCHPAD_FEATURE_UART |
        LAUNCHPAD_FEATURE_SPI |
        LAUNCHPAD_FEATURE_I2C |
        LAUNCHPAD_FEATURE_SDIO |
        LAUNCHPAD_FEATURE_CAN |        /* у ESP32 есть встроенный CAN контроллер */
        LAUNCHPAD_FEATURE_WIFI |
        LAUNCHPAD_FEATURE_BLE |
        LAUNCHPAD_FEATURE_IR |
        LAUNCHPAD_FEATURE_PWM |
        LAUNCHPAD_FEATURE_DAC |
        LAUNCHPAD_FEATURE_TOUCH |
        LAUNCHPAD_FEATURE_SD_CARD |
        LAUNCHPAD_FEATURE_FLASH |
        LAUNCHPAD_FEATURE_SLEEP_MODE |
        LAUNCHPAD_FEATURE_WAKEUP_GPIO |
        LAUNCHPAD_FEATURE_RTC_ALARM |
        LAUNCHPAD_FEATURE_HARDWARE_RNG |
        LAUNCHPAD_FEATURE_AES |
        LAUNCHPAD_FEATURE_HMAC |
        LAUNCHPAD_FEATURE_SECURE_BOOT |
        LAUNCHPAD_FEATURE_UART_DEBUG |
        LAUNCHPAD_FEATURE_WATCHDOG |
        LAUNCHPAD_FEATURE_SYSTEM_TICK,
    .hardware        =         LAUNCHPAD_HW_MULTICORE |
        LAUNCHPAD_HW_ARCH_XTENSA |
        LAUNCHPAD_HW_XTENSA_LX6 |

        /* Инструкционные расширения */
        LAUNCHPAD_HW_XTENSA_FPU |
        LAUNCHPAD_HW_XTENSA_CRYPTO |

        /* Микроархитектура */
        LAUNCHPAD_HW_CORE_INORDER |
        LAUNCHPAD_HW_CACHE_L1_I |
        LAUNCHPAD_HW_CACHE_L1_D |

        /* Безопасность */
        LAUNCHPAD_HW_RNG |
        LAUNCHPAD_HW_CRYPTO_ACCEL |

        /* Прочее */
        LAUNCHPAD_HW_HARDWARE_DIVIDE |
        LAUNCHPAD_HW_HARDWARE_MULTIPLY,
    /* Дополнительная информация – может быть пустой строкой */
    .custom_info     = ""
};

launchpad_platform_info_t launchpad_platform(void)
{
    /* Возвращаем копию (по соглашению ABI) */
    return g_launchpad_info;
}
