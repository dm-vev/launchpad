/* -------------------------------------------------------------
 * launchpad_log.c
 *
 * Wrapper API для логирования, чтобы ELF мог вызывать launchpad_log.
 * ------------------------------------------------------------- */

#include "include/log.h"
#include <esp_log.h>
#include <stdarg.h>
#include <stdio.h>

/* ------------------------------------------------------------------
 * Internal helper: map our level enum → esp_log_level_t
 * ------------------------------------------------------------------ */
static inline esp_log_level_t _to_esp_level(int level)
{
    switch (level) {
    case LAUNCHPAD_LOG_ERROR: return ESP_LOG_ERROR;
    case LAUNCHPAD_LOG_WARN:  return ESP_LOG_WARN;
    case LAUNCHPAD_LOG_INFO:  return ESP_LOG_INFO;
    case LAUNCHPAD_LOG_DEBUG: return ESP_LOG_DEBUG;
    case LAUNCHPAD_LOG_VERBOSE: return ESP_LOG_VERBOSE;
    default: return ESP_LOG_INFO;
    }
}

/* ------------------------------------------------------------------
 * Public API
 * ------------------------------------------------------------------ */

int launchpad_log(int level, const char *tag, const char *fmt, ...)
{
    if (!fmt) {
        return 1;
    }

    va_list args;
    va_start(args, fmt);

    esp_log_level_t esp_level = _to_esp_level(level);

    /* Передаём управление стандартному esp_log_writev */
    esp_log_writev(esp_level, tag ? tag : "ELF", fmt, args);

    va_end(args);
    return 0;
}
