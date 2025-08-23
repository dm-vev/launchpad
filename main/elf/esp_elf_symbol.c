/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <reent.h>
#include <pthread.h>
#include <setjmp.h>
#include <getopt.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <stdarg.h>

#include "rom/ets_sys.h"

#include "elf_symbol.h"

extern int __ltdf2(double a, double b);
extern unsigned int __fixunsdfsi(double a);
extern int __gtdf2(double a, double b);
extern double __floatunsidf(unsigned int i);
extern double __divdf3(double a, double b);

/** @brief Libc public functions symbols look-up table */

static const struct esp_elfsym g_esp_libc_elfsyms[] = {

    /* string.h */

    ESP_ELFSYM_EXPORT(strerror),
    ESP_ELFSYM_EXPORT(memset),
    ESP_ELFSYM_EXPORT(memcpy),
    ESP_ELFSYM_EXPORT(strlen),
    ESP_ELFSYM_EXPORT(strtod),
    ESP_ELFSYM_EXPORT(strrchr),
    ESP_ELFSYM_EXPORT(strchr),
    ESP_ELFSYM_EXPORT(strcmp),
    ESP_ELFSYM_EXPORT(strtol),
    ESP_ELFSYM_EXPORT(strcspn),
    ESP_ELFSYM_EXPORT(strncat),

    /* unistd.h */

    ESP_ELFSYM_EXPORT(usleep),
    ESP_ELFSYM_EXPORT(sleep),
    ESP_ELFSYM_EXPORT(exit),
    ESP_ELFSYM_EXPORT(close),

    /* stdlib.h */

    ESP_ELFSYM_EXPORT(malloc),
    ESP_ELFSYM_EXPORT(calloc),
    ESP_ELFSYM_EXPORT(realloc),
    ESP_ELFSYM_EXPORT(free),

    /* time.h */

    ESP_ELFSYM_EXPORT(clock_gettime),
    ESP_ELFSYM_EXPORT(strftime),

    /* pthread.h */

    ESP_ELFSYM_EXPORT(pthread_create),
    ESP_ELFSYM_EXPORT(pthread_attr_init),
    ESP_ELFSYM_EXPORT(pthread_attr_setstacksize),
    ESP_ELFSYM_EXPORT(pthread_detach),
    ESP_ELFSYM_EXPORT(pthread_join),
    ESP_ELFSYM_EXPORT(pthread_exit),

    /* newlib */

    ESP_ELFSYM_EXPORT(__errno),
    ESP_ELFSYM_EXPORT(__getreent),
#ifdef __HAVE_LOCALE_INFO__
    ESP_ELFSYM_EXPORT(__locale_ctype_ptr),
#else
    ESP_ELFSYM_EXPORT(_ctype_),
#endif

    /* math */

    ESP_ELFSYM_EXPORT(__ltdf2),
    ESP_ELFSYM_EXPORT(__fixunsdfsi),
    ESP_ELFSYM_EXPORT(__gtdf2),
    ESP_ELFSYM_EXPORT(__floatunsidf),
    ESP_ELFSYM_EXPORT(__divdf3),

    /* getopt.h */

    ESP_ELFSYM_EXPORT(getopt_long),
    ESP_ELFSYM_EXPORT(optind),
    ESP_ELFSYM_EXPORT(opterr),
    ESP_ELFSYM_EXPORT(optarg),
    ESP_ELFSYM_EXPORT(optopt),

    /* setjmp.h */

    ESP_ELFSYM_EXPORT(longjmp),
    ESP_ELFSYM_EXPORT(setjmp),

    ESP_ELFSYM_END
};

/** @brief ESP-IDF public functions symbols look-up table */

static const struct esp_elfsym g_esp_espidf_elfsyms[] = {

    /* sys/socket.h */

    ESP_ELFSYM_EXPORT(lwip_bind),
    ESP_ELFSYM_EXPORT(lwip_setsockopt),
    ESP_ELFSYM_EXPORT(lwip_socket),
    ESP_ELFSYM_EXPORT(lwip_listen),
    ESP_ELFSYM_EXPORT(lwip_accept),
    ESP_ELFSYM_EXPORT(lwip_recv),
    ESP_ELFSYM_EXPORT(lwip_recvfrom),
    ESP_ELFSYM_EXPORT(lwip_send),
    ESP_ELFSYM_EXPORT(lwip_sendto),
    ESP_ELFSYM_EXPORT(lwip_connect),

    /* arpa/inet.h */

    ESP_ELFSYM_EXPORT(ipaddr_addr),
    ESP_ELFSYM_EXPORT(lwip_htons),
    ESP_ELFSYM_EXPORT(lwip_htonl),
    ESP_ELFSYM_EXPORT(ip4addr_ntoa),

    /* ROM functions */

    ESP_ELFSYM_EXPORT(ets_printf),

    ESP_ELFSYM_END
};

/* Структура узла динамической таблицы */
struct dyn_esp_elfsym {
    const char *name;          /* строка‑имя символа                */
    void        *sym;           /* адрес функции/данных              */
    struct dyn_esp_elfsym *next;
};

/* Глобальный список новых символов (простой односвязный список) */
static struct dyn_esp_elfsym *g_dyn_syms = NULL;

/* Внутренняя реализация – добавление узла в список   */
uintptr_t _register_symbol(const char *name, void *sym)
{
    if (!name || !sym) {
        return 0;
    }

    /* Создаём новый узел */
    struct dyn_esp_elfsym *node = malloc(sizeof(*node));
    if (!node) {
        return 0;           /* нехватка памяти – просто отказываем */
    }

    /* Дублируем строку‑имя, чтобы она оставалась живой после выхода из функции */
    size_t len = strlen(name);
    char *name_copy = malloc(len + 1);
    if (!name_copy) {
        free(node);
        return 0;
    }
    memcpy(name_copy, name, len + 1);

    node->name = name_copy;
    node->sym  = sym;

    /* Вставляем в начало списка (не важно порядок) */
    node->next = g_dyn_syms;
    g_dyn_syms = node;

    return (uintptr_t)sym;   /* возвращаем адрес, чтобы можно было сразу использовать */
}

/* --------------------------------------------------------------------
   Перегрузка – вариант с явным именем
   ------------------------------------ */

static inline uintptr_t elf_register_new_named_symbol(const char *name,
                                                          void (*sym)(...))
{
    return _register_symbol(name, (void *)sym);
}

/* --------------------------------------------------------------------
   Макрос – вариант без имени (имя берётся из #operator)
   ------------------------------------------ */

#define ELF_REGISTER_NEW_SYMBOL(func) \
    _register_symbol(#func, (void *)(func))

/* Для удобства можно объявить макрос с тем же именем, что и функция */
#define elf_register_new_symbol(func)  ELF_REGISTER_NEW_SYMBOL(func)

/**
 * @brief Find symbol address by name.
 *
 * @param sym_name - Symbol name
 *
 * @return Symbol address if success or 0 if failed.
 */
uintptr_t elf_find_sym(const char *sym_name)
{
    const struct esp_elfsym *syms;

    syms = g_esp_libc_elfsyms;
    while (syms->name) {
        if (!strcmp(syms->name, sym_name)) {
            return (uintptr_t)syms->sym;
        }

        syms++;
    }

    syms = g_esp_libc_elfsyms;
    (void)syms;

    syms = g_esp_espidf_elfsyms;
    while (syms->name) {
        if (!strcmp(syms->name, sym_name)) {
            return (uintptr_t)syms->sym;
        }

        syms++;
    }

    syms = g_esp_espidf_elfsyms;
    (void)syms;

    /* Ищем в динамической таблице */
    struct dyn_esp_elfsym *node = g_dyn_syms;
    while (node) {
        if (!strcmp(node->name, sym_name)) {
            return (uintptr_t)node->sym;
        }
        node = node->next;
    }


    return 0;
}
