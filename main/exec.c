/* ──────────────────────────────────────────────── */
#include "exec.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>

#include "elf/esp_elf.h"
#include "esp_log.h"

static const char *TAG = "launchpad";

/*  exec_from_bytes – основная работа с esp_elf                       */
bool exec_from_bytes(const uint8_t *data, size_t size,
                     int argc, char **argv)
{
    esp_elf_t elf;
    esp_err_t err;

    /* Инициализируем структуру. */
    esp_elf_init(&elf);

    /* Перемещаем сегменты в RAM и готовим к выполнению. */
    err = esp_elf_relocate(&elf, data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
                 "esp_elf_relocate failed: %s",
                 esp_err_to_name(err));
        esp_elf_deinit(&elf);
        return false;
    }

    /* Запускаем ELF. Если нужно передать аргументы – передайте argc/argv. */
    err = esp_elf_request(&elf, 0, argc, argv);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
                 "esp_elf_request failed: %s",
                 esp_err_to_name(err));
        /* Ошибка не критична – ELF уже размещён в памяти. */
    }

    /* Очистка ресурсов. */
    esp_elf_deinit(&elf);
    return err == ESP_OK;
}

/*  exec_from_file – читаем файл и делаем вызов выше              */
bool exec_from_file(const char *path,
                    int argc, char **argv)
{
    /* Открываем ELF‑файл. */
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        ESP_LOGE(TAG, "Failed to open ELF file: %s", path);
        return false;
    }

    /* Получаем размер файла. */
    struct stat st;
    if (fstat(fd, &st) != 0) {
        close(fd);
        ESP_LOGE(TAG, "fstat failed on %s", path);
        return false;
    }

    /* Выделяем буфер и читаем файл полностью. */
    uint8_t *buffer = malloc(st.st_size);
    if (!buffer) {
        close(fd);
        ESP_LOGE(TAG, "Memory allocation failed");
        return false;
    }

    ssize_t bytes_read = read(fd, buffer, st.st_size);
    close(fd);

    if (bytes_read != st.st_size) {
        free(buffer);
        ESP_LOGE(TAG,
                 "Failed to read ELF file completely: %s",
                 path);
        return false;
    }

    /* Делегируем запуск. */
    bool ok = exec_from_bytes(buffer, st.st_size, argc, argv);

    free(buffer);
    return ok;
}
