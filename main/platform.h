/* --------------------------------------------------------------------- */
/*  launchpad_platform.h                                                */
/* --------------------------------------------------------------------- */

#ifndef LAUNCHPAD_PLATFORM_H
#define LAUNCHPAD_PLATFORM_H

#include <stdint.h>

/* ----------------------------------------------------------- */
/*  Ключевые константы и макросы                                */
/* ----------------------------------------------------------- */

/* Magic‑число – «проверка целостности» */
#define LAUNCHPAD_MAGIC          0x4C415046U   /* 'LAPF' */
#define HAS_FEATURE(info, feat)   (((info).features & (feat)) != 0)

/* Ориентация памяти */
enum {
    LAUNCHPAD_ENDIAN_LITTLE = 0,
    LAUNCHPAD_ENDIAN_BIG   = 1
};

/* Размеры строк (включая завершающий NUL) */
#define LAUNCHPAD_STR_LEN_LOADER      32U
#define LAUNCHPAD_STR_LEN_BUILD       64U
#define LAUNCHPAD_STR_LEN_PLATFORM    32U
#define LAUNCHPAD_STR_LEN_CUSTOM     128U

/* ----------------------------------------------------------- */
/*  Communication interfaces                                 */
/* ----------------------------------------------------------- */
enum {
    /* Classic serial ports ----------------------------------- */
    LAUNCHPAD_FEATURE_UART      = (1ULL << 0),   /* UART/USART  */
    LAUNCHPAD_FEATURE_SPI       = (1ULL << 1),   /* SPI bus     */
    LAUNCHPAD_FEATURE_I2C       = (1ULL << 2),   /* I²C bus     */
    LAUNCHPAD_FEATURE_SDIO      = (1ULL << 3),   /* SD‑IO slot  */

    /* USB and related -------------------------------------- */
    LAUNCHPAD_FEATURE_USB_HOST  = (1ULL << 4),   /* USB host OTG */
    LAUNCHPAD_FEATURE_USB_DEV   = (1ULL << 5),   /* USB device  */
    LAUNCHPAD_FEATURE_CAN       = (1ULL << 6),   /* CAN bus     */

    /* Wireless --------------------------------------------- */
    LAUNCHPAD_FEATURE_ETH      = (1ULL << 7),   /* Ethernet MAC */
    LAUNCHPAD_FEATURE_WIFI     = (1ULL << 8),   /* Wi‑Fi radio */
    LAUNCHPAD_FEATURE_BLE      = (1ULL << 9),   /* BLE stack   */
    LAUNCHPAD_FEATURE_NFC      = (1ULL <<10),   /* NFC reader/writer */
    LAUNCHPAD_FEATURE_LORA     = (1ULL <<11),   /* LoRaWAN     */
    LAUNCHPAD_FEATURE_ZIGBEE   = (1ULL <<12),   /* Zigbee      */

    /* Misc. ------------------------------------------------- */
    LAUNCHPAD_FEATURE_IR       = (1ULL <<13),   /* IR remote   */
    LAUNCHPAD_FEATURE_PWM      = (1ULL <<14),   /* PWM output  */
    LAUNCHPAD_FEATURE_DAC      = (1ULL <<15),    /* Digital‑to‑Analog */

    LAUNCHPAD_FEATURE_VTTY      = (1ULL << 16),   /* Virtual LaunchPad TTY  */
};

/* ----------------------------------------------------------- */
/*  Display / graphics interfaces                            */
/* ----------------------------------------------------------- */
enum {
    LAUNCHPAD_FEATURE_LCD       = (1ULL <<16),   /* TFT/LCD driver  */
    LAUNCHPAD_FEATURE_OLED      = (1ULL <<17),   /* OLED driver    */
    LAUNCHPAD_FEATURE_EPAPER    = (1ULL <<18),   /* e‑Paper        */
    LAUNCHPAD_FEATURE_LED_MATRIX= (1ULL <<19),   /* LED matrix     */
    LAUNCHPAD_FEATURE_GFX_ACCEL  = (1ULL <<20),   /* GPU / 2D/3D accel */

    /* Framebuffer support ---------------------------------- */
    LAUNCHPAD_FEATURE_FB_RGB565 = (1ULL <<21),   /* RGB565 framebuffer */
    LAUNCHPAD_FEATURE_FB_RGB888 = (1ULL <<22)    /* RGB888 framebuffer */
};

/* ----------------------------------------------------------- */
/*  Audio interfaces                                          */
/* ----------------------------------------------------------- */
enum {
    LAUNCHPAD_FEATURE_I2S       = (1ULL <<23),   /* I²S audio bus */
    LAUNCHPAD_FEATURE_AUDIO_DAC = (1ULL <<24),   /* DAC for speaker */
    LAUNCHPAD_FEATURE_AUDIO_ADC = (1ULL <<25)    /* ADC for mic */
};

/* ----------------------------------------------------------- */
/*  Sensors & actuators                                      */
/* ----------------------------------------------------------- */
enum {
    /* Inertial measurement unit ---------------------------------*/
    LAUNCHPAD_FEATURE_ACCEL     = (1ULL <<26),   /* Accelerometer  */
    LAUNCHPAD_FEATURE_GYRO      = (1ULL <<27),   /* Gyroscope      */
    LAUNCHPAD_FEATURE_MAG       = (1ULL <<28),   /* Magnetometer   */

    /* Environmental ------------------------------------------*/
    LAUNCHPAD_FEATURE_TEMP      = (1ULL <<29),   /* Temperature sensor  */
    LAUNCHPAD_FEATURE_HUMID     = (1ULL <<30),   /* Humidity sensor     */
    LAUNCHPAD_FEATURE_PRESS     = (1ULL <<31),   /* Pressure sensor     */

    /* Other physical sensors ---------------------------------*/
    LAUNCHPAD_FEATURE_TOUCH     = (1ULL <<32),   /* Capacitive touch  */
    LAUNCHPAD_FEATURE_PROXIMITY = (1ULL <<33),   /* Proximity sensor  */
    LAUNCHPAD_FEATURE_LIGHT     = (1ULL <<34),   /* Ambient light     */

    /* Camera interface ---------------------------------------*/
    LAUNCHPAD_FEATURE_CAMERA_MIPI = (1ULL <<35), /* MIPI‑CSI camera  */
    LAUNCHPAD_FEATURE_CAMERA_USB  = (1ULL <<36)  /* USB camera       */
};

/* ----------------------------------------------------------- */
/*  Storage & memory                                         */
/* ----------------------------------------------------------- */
enum {
    LAUNCHPAD_FEATURE_FLASH      = (1ULL <<37),   /* Internal flash   */
    LAUNCHPAD_FEATURE_SD_CARD    = (1ULL <<38),   /* SD card          */
    LAUNCHPAD_FEATURE_NAND       = (1ULL <<39),   /* NAND flash       */
    LAUNCHPAD_FEATURE_EEPROM     = (1ULL <<40),   /* EEPROM           */
    LAUNCHPAD_FEATURE_FRAM       = (1ULL <<41)    /* FRAM             */
};

/* ----------------------------------------------------------- */
/*  Power management                                         */
/* ----------------------------------------------------------- */
enum {
    LAUNCHPAD_FEATURE_BATTERY_CHARGER = (1ULL <<42), /* Battery charger */
    LAUNCHPAD_FEATURE_DCDC_CONVERTER  = (1ULL <<43), /* Buck/boost     */
    LAUNCHPAD_FEATURE_SLEEP_MODE      = (1ULL <<44), /* Low‑power sleep*/
    LAUNCHPAD_FEATURE_WAKEUP_GPIO     = (1ULL <<45), /* GPIO wake‑up   */
    LAUNCHPAD_FEATURE_RTC_ALARM       = (1ULL <<46)  /* RTC alarm      */
};

/* ----------------------------------------------------------- */
/*  Security & cryptography                                  */
/* ----------------------------------------------------------- */
enum {
    LAUNCHPAD_FEATURE_SECURE_BOOT     = (1ULL <<47), /* Secure boot   */
    LAUNCHPAD_FEATURE_HARDWARE_RNG    = (1ULL <<48), /* Hardware RNG  */
    LAUNCHPAD_FEATURE_AES             = (1ULL <<49), /* AES engine    */
    LAUNCHPAD_FEATURE_RSA             = (1ULL <<50), /* RSA/ECDSA     */
    LAUNCHPAD_FEATURE_HMAC            = (1ULL <<51), /* HMAC          */
    LAUNCHPAD_FEATURE_TAMPER_DETECT  = (1ULL <<52)  /* Tamper detect*/
};

/* ----------------------------------------------------------- */
/*  Debug / trace / system services                          */
/* ----------------------------------------------------------- */
enum {
    LAUNCHPAD_FEATURE_JTAG            = (1ULL <<53),   /* JTAG/SWD      */
    LAUNCHPAD_FEATURE_UART_DEBUG     = (1ULL <<54),   /* UART console  */
    LAUNCHPAD_FEATURE_TRACE_PORTS    = (1ULL <<55),   /* SWO/ITM       */
    LAUNCHPAD_FEATURE_WATCHDOG       = (1ULL <<56),   /* WWDG/IWDG      */
    LAUNCHPAD_FEATURE_SYSTEM_TICK    = (1ULL <<57),   /* SysTick timer  */
    LAUNCHPAD_FEATURE_EVENT_COUNTER  = (1ULL <<58)    /* Event counters */
};

typedef uint64_t launchpad_hardware_t;

/* ----------------------------------------------------------- */
/*  0–1. Количество ядер -----------------------------------*/
enum {
    LAUNCHPAD_HW_MULTICORE      = (1ULL << 0),   /* более одного ядра       */
    LAUNCHPAD_HW_SINGLECORE     = (1ULL << 1)    /* только одно ядро        */
};

/* ----------------------------------------------------------- */
/*  2–6. Архитектура --------------------------------------*/
enum {
    LAUNCHPAD_HW_ARCH_ARM      = (1ULL << 2),   /* ARM (Cortex‑M, Cortex‑A…) */
    LAUNCHPAD_HW_ARCH_XTENSA   = (1ULL << 3),   /* Xtensa (ESP32, ESP32‑S)      */
    LAUNCHPAD_HW_ARCH_RISCV    = (1ULL << 4),   /* RISC‑V (RV32I/64I…)          */
    LAUNCHPAD_HW_ARCH_X86      = (1ULL << 5),   /* x86/x64                      */
    LAUNCHPAD_HW_ARCH_POWER    = (1ULL << 6),   /* PowerPC (PPC32/64)           */
    LAUNCHPAD_HW_ARCH_WM       = (1ULL << 7)    /* WinnerMicro MCU (W806…)       */
};

/* ----------------------------------------------------------- */
/*  8–17. Конкретные ядра -----------------------------------*/
enum {
    /* ARM Cortex‑M family   */
    LAUNCHPAD_HW_ARM_CORTEX_M0   = (1ULL << 8),   /* M0, M0+          */
    LAUNCHPAD_HW_ARM_CORTEX_M3   = (1ULL << 9),   /* M3               */
    LAUNCHPAD_HW_ARM_CORTEX_M4   = (1ULL <<10),   /* M4 / M4F         */
    LAUNCHPAD_HW_ARM_CORTEX_M7   = (1ULL <<11),   /* M7 / M7F         */
    LAUNCHPAD_HW_ARM_CORTEX_A53  = (1ULL <<12),   /* A53              */
    LAUNCHPAD_HW_ARM_CORTEX_A57  = (1ULL <<13),   /* A57              */
    LAUNCHPAD_HW_ARM_CORTEX_A72  = (1ULL <<14),   /* A72              */

    /* Xtensa LX family     */
    LAUNCHPAD_HW_XTENSA_LX6      = (1ULL <<15),   /* ESP32            */
    LAUNCHPAD_HW_XTENSA_LX7      = (1ULL <<16),   /* ESP32‑S          */

    /* RISC‑V RV32I / RV64I  */
    LAUNCHPAD_HW_RISCV_RV32I     = (1ULL <<17)    /* RV32I (base I)   */
};

/* ----------------------------------------------------------- */
/* 18–23. Дополнительные ядра --------------------------------*/
enum {
    LAUNCHPAD_HW_RISCV_RV64I     = (1ULL <<18),   /* RV64I            */
    LAUNCHPAD_HW_X86_32          = (1ULL <<19),   /* x86‑32           */
    LAUNCHPAD_HW_X86_64          = (1ULL <<20),   /* x86‑64           */
    LAUNCHPAD_HW_POWER_PPC32     = (1ULL <<21),   /* PowerPC 32‑bit   */
    LAUNCHPAD_HW_POWER_PPC64     = (1ULL <<22),   /* PowerPC 64‑bit   */
    LAUNCHPAD_HW_WM_W806        = (1ULL <<23)    /* WinnerMicro W806  */
};

/* ----------------------------------------------------------- */
/* 24–33. Инструкционные расширения ------------------------*/
enum {
    /* ARM extensions --------------------------------------*/
    LAUNCHPAD_HW_ARM_NEON        = (1ULL <<24),   /* SIMD – NEON     */
    LAUNCHPAD_HW_ARM_VFP         = (1ULL <<25),   /* VFP FPU          */
    LAUNCHPAD_HW_ARM_DSP         = (1ULL <<26),   /* DSP расширения  */

    /* Xtensa extensions -----------------------------------*/
    LAUNCHPAD_HW_XTENSA_FPU      = (1ULL <<27),   /* Floating‑point  */
    LAUNCHPAD_HW_XTENSA_CRYPTO   = (1ULL <<28),   /* Crypto engine    */

    /* RISC‑V extensions -----------------------------------*/
    LAUNCHPAD_HW_RISCV_C        = (1ULL <<29),   /* compressed instr.*/
    LAUNCHPAD_HW_RISCV_F        = (1ULL <<30),   /* FP support      */
    LAUNCHPAD_HW_RISCV_A        = (1ULL <<31),   /* atomic ops      */

    /* x86 extensions -------------------------------------*/
    LAUNCHPAD_HW_X86_SSE2       = (1ULL <<32),   /* SSE2            */
    LAUNCHPAD_HW_X86_AVX        = (1ULL <<33)    /* AVX             */
};

/* ----------------------------------------------------------- */
/* 34–42. Микроархитектура / производительность -------------*/
enum {
    LAUNCHPAD_HW_CORE_INORDER     = (1ULL <<34),   /* in‑order exec      */
    LAUNCHPAD_HW_CORE_OUTOFORDER  = (1ULL <<35),   /* out‑of‑order exec  */
    LAUNCHPAD_HW_CORE_SUPERSCALAR  = (1ULL <<36),   /* superscalar width  */
    LAUNCHPAD_HW_CORE_BRANCH_PRED  = (1ULL <<37),   /* branch predictor   */

    LAUNCHPAD_HW_CACHE_L1_I        = (1ULL <<38),   /* L1 instruction cache */
    LAUNCHPAD_HW_CACHE_L1_D        = (1ULL <<39),   /* L1 data cache       */
    LAUNCHPAD_HW_CACHE_L2          = (1ULL <<40),   /* L2 cache            */
    LAUNCHPAD_HW_CACHE_L3          = (1ULL <<41),   /* L3 cache            */
    LAUNCHPAD_HW_TLB_64KB          = (1ULL <<42)    /* 64 KiB TLB entry    */
};

/* ----------------------------------------------------------- */
/* 43–51. Безопасность / виртуализация ---------------------*/
enum {
    LAUNCHPAD_HW_SECURE_TZ        = (1ULL <<43),   /* TrustZone / S‑S      */
    LAUNCHPAD_HW_VIRT_SUPPORT     = (1ULL <<44),   /* VT‑x/AMD‑V           */
    LAUNCHPAD_HW_HSM              = (1ULL <<45),   /* Hardware Security Module */
    LAUNCHPAD_HW_RNG               = (1ULL <<46),   /* HW random number gen. */
    LAUNCHPAD_HW_CRYPTO_ACCEL     = (1ULL <<47),   /* Crypto accelerator   */
    LAUNCHPAD_HW_POWER_MANAGEMENT  = (1ULL <<48),   /* Deep‑sleep, wake‑up   */
    LAUNCHPAD_HW_MEMORY_ORDERING   = (1ULL <<49),   /* Strong memory ordering? */
    LAUNCHPAD_HW_INSTRUCTION_CACHE= (1ULL <<50),   /* Instruction cache present */
    LAUNCHPAD_HW_DATA_CACHE       = (1ULL <<51)    /* Data cache present    */
};

/* ----------------------------------------------------------- */
/* 52–63. Прочие и расширяемые фичи ------------------------*/
enum {
    LAUNCHPAD_HW_MULTIPLICATION_FPU = (1ULL <<52),   /* FP multiplier      */
    LAUNCHPAD_HW_DIVISION_FPU       = (1ULL <<53),   /* FP divider        */
    LAUNCHPAD_HW_SIMD_SUPPORT       = (1ULL <<54),   /* SIMD present      */
    LAUNCHPAD_HW_VECTOR_SUPPORT     = (1ULL <<55),   /* Vector extension  */
    LAUNCHPAD_HW_HARDWARE_DIVIDE    = (1ULL <<56),   /* HW divide unit    */
    LAUNCHPAD_HW_HARDWARE_MULTIPLY   = (1ULL <<57),   /* HW multiply unit  */
    LAUNCHPAD_HW_INSTRUCTION_CACHE_PRESENT = (1ULL <<58),
    LAUNCHPAD_HW_DATA_CACHE_PRESENT      = (1ULL <<59),
    /* Виртуализация / контейнеры ------------------------------------*/
    LAUNCHPAD_HW_IS_VM                     = (1ULL << 60),   /* код внутри LaunchPad VM */
    LAUNCHPAD_HW_IS_CONTAINER               = (1ULL << 61),   /* запущен в контейнере (Docker, runc…) */

    /* Возможности хоста для виртуализации --------------------------*/
    LAUNCHPAD_HW_VIRTUALIZATION_SUPPORTED   = (1ULL << 62),   /* CPU/firmware поддерживает гипервизор (VT‑x/SVM) */
    LAUNCHPAD_HW_SECURE_VM                 = (1ULL << 63)    /* защищённый VM / TE (TrustZone, SGX, SEV…) */
};


/* ----------------------------------------------------------- */
/*  Структура ABI                                            */
/* ----------------------------------------------------------- */

typedef struct __attribute__((aligned(8))) launchpad_platform_info_t {
    uint32_t magic;                     /* 0x4C415046 – sanity check          */
    uint16_t version_major;             /* Major part of loader version        */
    uint16_t version_minor;             /* Minor part                          */
    uint16_t version_patch;             /* Patch level                         */
    uint32_t build_timestamp;           /* Unix epoch or yyyymmdd (4‑byte)     */

    char loader_name[LAUNCHPAD_STR_LEN_LOADER];   /* "ESP32-C3 Loader"          */
    char build_name [LAUNCHPAD_STR_LEN_BUILD ];   /* "Release Build"            */
    char platform_name[LAUNCHPAD_STR_LEN_PLATFORM];/* "ESP32-C3"                */

    uint8_t bitness;                    /* 32 or 64                           */
    uint8_t endian;                     /* 0 = little, 1 = big                 */
    uint16_t reserved0;                 /* padding to align next field (4‑byte)*/

    uint64_t features;                  /* Bitmask of supported features        */
    uint64_t hardware;

    char custom_info[LAUNCHPAD_STR_LEN_CUSTOM];/* Vendor‑specific string           */
} launchpad_platform_info_t;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Возвращает структуру с информацией о загрузчике.
 *
 * В большинстве случаев функция просто возвращает копию статической константы,
 * но можно реализовать более сложную логику (например, считывать из EEPROM).
 */
launchpad_platform_info_t launchpad_platform(void);

#ifdef __cplusplus
}
#endif

#endif /* LAUNCHPAD_PLATFORM_H */
