/* ──────────────────────────────────────────────── */
#ifndef EXEC_H
#define EXEC_H

#include <stdbool.h>
#include <stddef.h>   /* size_t */

/**
 * @brief Запускает ELF из памяти.
 *
 * @param data    указатель на бинарный образ ELF
 * @param size    размер данных в байтах
 * @param argc    число аргументов (можно 0)
 * @param argv    массив строк-аргументов (можно NULL)
 * @return true   – ELF успешно запущен
 *         false  – ошибка
 */
bool exec_from_bytes(const uint8_t *data, size_t size,
                     int argc, char **argv);

/**
 * @brief Читает файл по `path` и запускает его как ELF.
 *
 * @param path    путь к файлу (например, "/boot/app.elf")
 * @param argc    число аргументов
 * @param argv    массив строк-аргументов
 * @return true   – ELF успешно запущен
 *         false  – ошибка (файл не найден/не прочитан/плохой ELF)
 */
bool exec_from_file(const char *path,
                    int argc, char **argv);

#endif /* EXEC_H */
/* ──────────────────────────────────────────────── */
