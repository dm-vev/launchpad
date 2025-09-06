/* -------------------------------------------------------------
 * launchpad_processor_api.h
 *
 * Lightweight wrappers around ESP-IDF CPU/system control APIs.
 * All functions return **0 on success** and a non-zero value on error,
 * unless noted otherwise.
 * ------------------------------------------------------------- */

#ifndef LAUNCHPAD_PROCESSOR_API_H
#define LAUNCHPAD_PROCESSOR_API_H

#include <stdint.h>
#include <stdbool.h>
#include "esp_system.h"
#include "esp_sleep.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --- Базовые функции управления --- */
void launchpad_reboot(void);
void launchpad_halt(void);
int  launchpad_sleep(void);
int  launchpad_deep_sleep(uint64_t us);
int  launchpad_set_stack_pointer(uint32_t sp);
int  launchpad_jump_to(uint32_t addr);

/* --- Прерывания и векторы --- */
void launchpad_irq_enable(void);
void launchpad_irq_disable(void);
void launchpad_send_ipi(int core_id);
int  launchpad_set_vector_table(uint32_t addr);
int  launchpad_trigger_interrupt(int irq_num);

/* --- Диагностика CPU --- */
uint32_t launchpad_get_pc(void);
int      launchpad_get_cpu_id(uint32_t *out_id);
int      launchpad_get_reset_reason(void);
uint32_t launchpad_get_cpu_freq(void);

/* --- Новые: управление частотой и ядрами --- */
int  launchpad_set_cpu_freq(uint32_t hz);
int  launchpad_power_down_core(int core_id);
int  launchpad_wakeup_core(int core_id);
int  launchpad_halt_core(int core_id);
int  launchpad_get_core_id(void);

/* --- Watchdog --- */
int  launchpad_watchdog_init(uint32_t timeout_ms);
void launchpad_watchdog_feed(void);

/* --- Cache / Heap --- */
void     launchpad_cache_flush(void);
int      launchpad_cache_enable(void);
int      launchpad_cache_disable(void);
uint32_t launchpad_get_free_heap(void);
uint32_t launchpad_get_total_heap(void);

/* --- Security / eFuse --- */
bool     launchpad_is_secure_boot_enabled(void);
bool     launchpad_is_flash_encryption_enabled(void);
uint32_t launchpad_efuse_read_word(int index);

/* --- Диагностика --- */
uint64_t launchpad_get_cycle_count(void);
uint64_t launchpad_get_uptime_ms(void);
int      launchpad_get_temperature(void);

#ifdef __cplusplus
}
#endif

#endif /* LAUNCHPAD_PROCESSOR_API_H */
