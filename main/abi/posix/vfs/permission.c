#include <errno.h>
#include <string.h>    /* strdup() */
#include <libgen.h>    /* dirname() – если доступно в ESP-IDF */
#include <fcntl.h>

#ifdef CONFIG_POSIX_SUPPORT_ENABLED
#   include <unistd.h>       /* access(), chmod() */
#else
/* В случае отключённой POSIX‑поддержки подключаем VFS API,
 * если он есть.  Если его нет, мы просто будем возвращать ENOSYS.
 */
#   include "esp_vfs.h"
#   include "esp_err.h"     /* esp_err_to_errno() */
#endif

/* --------------------------------------------------------------------- */
/* Проверка прав доступа (тот же код, что в предыдущем примере)           */

int launchpad_vfs_permission(const char *path, int flags)
{
    if (!path) {
        errno = EINVAL;
        return -1;
    }

    /* --- Преобразуем флаги O_* в режим для access() ----------------- */
    int mode = 0;

    /* Проверяем наличие файла (если не указан F_OK – просто смотрим существование) */
    if ((flags & O_CREAT) && (access(path, F_OK) != 0)) {
        /* Файл ещё не существует. Для создания нужна запись и
         * исполнение в родительской директории.
         */
        char *tmp = strdup(path);          // копия пути – dirname() меняет строку
        if (!tmp) {
            errno = ENOMEM;
            return -1;
        }

        /* Получаем путь к родителю */
#ifdef LIBGEN_H_EXISTS   /* если libgen.h доступен в ESP-IDF */
        char *parent = dirname(tmp);       // parent может быть "/"
#else
        /* Самодельный fallback: ищем последний '/' и обрезаем строку */
        char *last_slash = strrchr(tmp, '/');
        if (last_slash && last_slash != tmp) {
            *last_slash = '\0';
        } else {
            /* Путь вида "file" – родительская директория это "." */
            strcpy(tmp, ".");
        }
        char *parent = tmp;
#endif

        /* Проверяем права на запись и исполнение в родительской директории */
        int rc = access(parent, W_OK | X_OK);
        free(tmp);
        return rc;          // 0 – OK, -1 – ошибка (errno уже установлен)
    }

    /* Файл существует: определяем требуемые режимы чтения/записи. */
    if (flags & O_RDONLY) {
        mode |= R_OK;
    }
    if ((flags & O_WRONLY) || (flags & O_RDWR)) {
        mode |= W_OK;
    }

    /* Если флаг O_EXCL – требуется, чтобы файл НЕ существовал.
     * Это не проверяется access(), но можно добавить дополнительную логику,
     * если понадобится. */
    return access(path, mode);
}

/* --------------------------------------------------------------------- */
/* Установка прав доступа ------------------------------------------------*/

int launchpad_set_permission(const char *path, mode_t mode)
{
    if (!path) {
        errno = EINVAL;
        return -1;
    }


# ifdef CONFIG_VFS_SUPPORT
    /* В VFS API есть esp_vfs_chmod(). Если оно доступно – используем. */
#  if defined(ESP_IDF_VERSION) && ESP_IDF_VERSION >= 50000   /* 5.x и выше */
    int rc = esp_vfs_chmod(path, mode);
    if (rc != 0) {
        errno = esp_err_to_errno(rc);      /* esp_err_t → POSIX errno */
        return -1;
    }
    return 0;

#  else   /* старые версии SDK без esp_vfs_chmod() */
    /* Функция недоступна – возвращаем ошибку ENOSYS. */
    errno = ENOSYS;
    return -1;
#  endif

# else   /* CONFIG_VFS_SUPPORT не включён вообще */
    /* Ни POSIX, ни VFS: ничего делать нельзя. */
    errno = ENOSYS;
    return -1;
# endif
#endif
}