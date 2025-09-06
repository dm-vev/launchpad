#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/* Уровни логирования для ELF */
enum {
    LAUNCHPAD_LOG_ERROR   = 1,
    LAUNCHPAD_LOG_WARN    = 2,
    LAUNCHPAD_LOG_INFO    = 3,
    LAUNCHPAD_LOG_DEBUG   = 4,
    LAUNCHPAD_LOG_VERBOSE = 5,
};

/**
 * @brief Логирование из ELF.
 *
 * @param level Уровень (см. enum)
 * @param tag   Строка-тег (может быть NULL → "ELF")
 * @param fmt   Форматная строка (printf-style)
 * @param ...   Аргументы
 *
 * @return 0 при успехе, 1 при ошибке
 */
int launchpad_log(int level, const char *tag, const char *fmt, ...);

#ifdef __cplusplus
}
#endif
