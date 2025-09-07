/* -------------------------------------------------------------
 * launchpad_processor_api.c
 *
 * Implementation of lightweight CPU/system control wrappers
 * for ESP32 devices (ESP-IDF).
 * ------------------------------------------------------------- */

#include "include/processor.h"
#include "esp_chip_info.h"
#include "esp_timer.h"
#include "esp_system.h"
#include "esp_heap_caps.h"
#include "esp_efuse.h"
#include "esp_secure_boot.h"
#include "esp_flash_encrypt.h"
#include "esp_efuse.h"
#include "esp_efuse_table.h"
#include <string.h>

#include "soc/soc.h"

void launchpad_send_ipi(int core_id) {
#if defined(__XTENSA__)
    if (core_id == 0) {
        REG_WRITE(DPORT_CPU_INTR_FROM_CPU_0_REG, 1);
    } else if (core_id == 1) {
        REG_WRITE(DPORT_CPU_INTR_FROM_CPU_1_REG, 1);
    }
#elif defined(__riscv)
    volatile uint32_t *msip = (uint32_t *)(0x02000000UL + core_id * 4);
    *msip = 1;
#else
    (void)core_id;
#endif
}


void launchpad_reboot(void) {
    esp_restart();
}

void launchpad_halt(void) {
    while (1) {
#if defined(__XTENSA__)
        __asm__ __volatile__("waiti 0");
#elif defined(__riscv)
        __asm__ __volatile__("wfi");
#endif
    }
}

int launchpad_sleep(void) {
    return esp_light_sleep_start();
}

int launchpad_deep_sleep(uint64_t us) {
    esp_sleep_enable_timer_wakeup(us);
    esp_deep_sleep_start();
    return 0;
}

int launchpad_set_stack_pointer(uint32_t sp) {
#if defined(__XTENSA__)
    __asm__ __volatile__("mov a1, %0\n" :: "r"(sp));
    return 0;
#elif defined(__riscv)
    __asm__ __volatile__("mv sp, %0\n" :: "r"(sp));
    return 0;
#else
    return -1;
#endif
}

int launchpad_jump_to(uint32_t addr) {
    launchpad_irq_disable();
#if defined(__XTENSA__)
    __asm__ __volatile__("jx %0\n" :: "r"(addr));
#elif defined(__riscv)
    __asm__ __volatile__("jr %0\n" :: "r"(addr));
#else
    return -1;
#endif
    return 0;
}

void launchpad_irq_enable(void) {
#if defined(__XTENSA__)
    __asm__ __volatile__("rsil a2, 0");
#elif defined(__riscv)
    __asm__ __volatile__("csrsi mstatus, 0x8");
#endif
}

void launchpad_irq_disable(void) {
#if defined(__XTENSA__)
    __asm__ __volatile__("rsil a2, 15");
#elif defined(__riscv)
    __asm__ __volatile__("csrci mstatus, 0x8");
#endif
}

int launchpad_set_vector_table(uint32_t addr) {
#if defined(__XTENSA__)
    __asm__ __volatile__("wsr.vecbase %0\n" :: "r"(addr));
    return 0;
#elif defined(__riscv)
    __asm__ __volatile__("csrw mtvec, %0\n" :: "r"(addr));
    return 0;
#else
    return -1;
#endif
}

int launchpad_trigger_interrupt(int irq_num) {
    (void)irq_num;
    return -1; // TODO: реализовать через PLIC/CLINT (RISC-V)
}

uint32_t launchpad_get_pc(void) {
    uint32_t pc = 0;
#if defined(__XTENSA__)
    __asm__ __volatile__("mov %0, a0" : "=r"(pc));
#elif defined(__riscv)
    __asm__ __volatile__("auipc %0, 0" : "=r"(pc));
#endif
    return pc;
}

int launchpad_get_cpu_id(uint32_t *out_id) {
    if (!out_id) return -1;
    esp_chip_info_t info;
    esp_chip_info(&info);
    *out_id = (info.model << 16) | (info.revision);
    return 0;
}

int launchpad_get_reset_reason(void) {
    return (int)esp_reset_reason();
}

uint32_t launchpad_get_cpu_freq(void) {
    return 100;
}

/* --- Новые функции --- */

int launchpad_set_cpu_freq(uint32_t hz) {
    return 1;
}

int launchpad_power_down_core(int core_id) {
    (void)core_id;
    return -1; // ESP32 SMP не поддерживает power-down ядра напрямую
}

int launchpad_wakeup_core(int core_id) {
    (void)core_id;
    return -1;
}

int launchpad_halt_core(int core_id) {
    (void)core_id;
    return -1;
}

int launchpad_get_core_id(void) {
#if defined(__XTENSA__)
    uint32_t id;
    __asm__ __volatile__("rsr.prid %0" : "=r"(id));
    return id & 0x1;
#elif defined(__riscv)
    uint32_t id;
    __asm__ __volatile__("csrr %0, mhartid" : "=r"(id));
    return (int)id;
#else
    return -1;
#endif
}

int launchpad_watchdog_init(uint32_t timeout_ms) {
    return 0;
}

void launchpad_watchdog_feed(void) {}

void launchpad_cache_flush(void) {
    // В ESP-IDF cache управляется драйвером SPI Flash
    // Здесь оставляем заглушку
}

int launchpad_cache_enable(void) {
    return 0; // всегда включён
}

int launchpad_cache_disable(void) {
    return -1; // выключать нельзя из user-space
}

uint32_t launchpad_get_free_heap(void) {
    return esp_get_free_heap_size();
}

uint32_t launchpad_get_total_heap(void) {
    return heap_caps_get_total_size(MALLOC_CAP_DEFAULT);
}

bool launchpad_is_secure_boot_enabled(void) {
#ifdef CONFIG_SECURE_BOOT
    return esp_secure_boot_enabled();
#else
    return false;
#endif
}

bool launchpad_is_flash_encryption_enabled(void) {
    return esp_flash_encryption_enabled();
}

uint32_t launchpad_efuse_read_word_block(int word_index) {
    return esp_efuse_read_reg(EFUSE_BLK0, word_index);
}


uint64_t launchpad_get_cycle_count(void) {
#if defined(__XTENSA__)
    uint32_t ccount;
    __asm__ __volatile__("rsr.ccount %0" : "=r"(ccount));
    return (uint64_t)ccount;
#elif defined(__riscv)
    uint64_t cycles;
    __asm__ __volatile__("rdcycle %0" : "=r"(cycles));
    return cycles;
#else
    return 0;
#endif
}

uint64_t launchpad_get_uptime_ms(void) {
    return esp_timer_get_time() / 1000ULL;
}

int launchpad_get_temperature(void) {
    return -1;
}
