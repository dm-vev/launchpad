#include "launchpad_vtty.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef LAUNCHPAD_VTTY_MAX_DRIVERS
#define LAUNCHPAD_VTTY_MAX_DRIVERS 8
#endif

static const struct vtty_driver *g_drivers[LAUNCHPAD_VTTY_MAX_DRIVERS];
static struct launchpad_vtty_info g_infos[LAUNCHPAD_VTTY_MAX_DRIVERS];
static int g_driver_count = 0;
static int g_current_index = -1;
static launchpad_vtty_event_cb_t g_event_cb = NULL;

/* -------------------------------------------------------------------------- */
/* Default stdio driver                                                       */
/* -------------------------------------------------------------------------- */

static int stdio_putc(char c)
{
    return (putchar((int)c) == EOF) ? -1 : 0;
}

static int stdio_puts(const char *s)
{
    if (!s)
        return -1;
    if (fputs(s, stdout) < 0)
        return -1;
    return (int)strlen(s);
}

static void stdio_flush(void)
{
    fflush(stdout);
}

static int stdio_getc(void)
{
    int c = getchar();
    return (c == EOF) ? -1 : c;
}

static int stdio_available(void)
{
    return 1; /* stdin is always available in this stub */
}

static void stdio_clear_screen(void)
{
    fputs("\x1B[2J", stdout);
}

static void stdio_move_cursor(int row, int col)
{
    fprintf(stdout, "\x1B[%d;%dH", row, col);
}

static int stdio_is_ready(void)
{
    return 1;
}

static const struct vtty_driver stdio_driver = {
    .id = 0,
    .type = "stdio",
    .init = NULL,
    .deinit = NULL,
    .putc = stdio_putc,
    .puts = stdio_puts,
    .flush = stdio_flush,
    .getc = stdio_getc,
    .available = stdio_available,
    .clear_screen = stdio_clear_screen,
    .move_cursor = stdio_move_cursor,
    .set_baudrate = NULL,
    .is_ready = stdio_is_ready,
    .set_callback = NULL,
    .ioctl = NULL,
};

/* -------------------------------------------------------------------------- */
/* Core management                                                            */
/* -------------------------------------------------------------------------- */

int launchpad_vtty_register_driver(const struct vtty_driver *drv)
{
    if (!drv || g_driver_count >= LAUNCHPAD_VTTY_MAX_DRIVERS)
        return -1;

    g_drivers[g_driver_count] = drv;
    g_infos[g_driver_count].id = drv->id;
    g_infos[g_driver_count].type = drv->type;

    if (drv->init)
        drv->init();

    g_driver_count++;
    return 0;
}

int launchpad_vtty_set_default(int id)
{
    for (int i = 0; i < g_driver_count; ++i) {
        if (g_infos[i].id == id) {
            g_current_index = i;
            return 0;
        }
    }
    return -1;
}

struct launchpad_vtty_info launchpad_vtty_get_current(void)
{
    struct launchpad_vtty_info info = { .id = -1, .type = NULL };
    if (g_current_index >= 0 && g_current_index < g_driver_count)
        info = g_infos[g_current_index];
    return info;
}

const struct launchpad_vtty_info *launchpad_vtty_list(int *cnt)
{
    if (cnt)
        *cnt = g_driver_count;
    return g_infos;
}

int launchpad_vtty_init(void)
{
    g_driver_count = 0;
    g_current_index = -1;
    g_event_cb = NULL;
    launchpad_vtty_register_driver(&stdio_driver);
    launchpad_vtty_register_uart(1, 38, 48, 115200);
    launchpad_vtty_set_default(1);
    return 0;
}

int launchpad_vtty_deinit(void)
{
    for (int i = 0; i < g_driver_count; ++i) {
        if (g_drivers[i] && g_drivers[i]->deinit)
            g_drivers[i]->deinit();
    }
    g_driver_count = 0;
    g_current_index = -1;
    g_event_cb = NULL;
    return 0;
}

/* -------------------------------------------------------------------------- */
/* Output operations                                                          */
/* -------------------------------------------------------------------------- */

static const struct vtty_driver *current_driver(void)
{
    if (g_current_index < 0 || g_current_index >= g_driver_count)
        return NULL;
    return g_drivers[g_current_index];
}

int launchpad_vtty_putc(char c)
{
    const struct vtty_driver *drv = current_driver();
    if (!drv || !drv->putc)
        return -1;
    return drv->putc(c);
}

int launchpad_vtty_putchar(char c)
{
    return launchpad_vtty_putc(c);
}

int launchpad_vtty_puts(const char *s)
{
    const struct vtty_driver *drv = current_driver();
    if (!drv)
        return -1;
    if (drv->puts)
        return drv->puts(s);
    /* Fallback using putc */
    int count = 0;
    while (s && *s) {
        if (drv->putc)
            drv->putc(*s++);
        else
            return -1;
        count++;
    }
    return count;
}

int launchpad_vtty_printf(const char *fmt, ...)
{
    char buffer[256];
    va_list ap;
    va_start(ap, fmt);
    int len = vsnprintf(buffer, sizeof(buffer), fmt, ap);
    va_end(ap);
    if (len < 0)
        return -1;
    return launchpad_vtty_puts(buffer);
}

void launchpad_vtty_flush(void)
{
    const struct vtty_driver *drv = current_driver();
    if (drv && drv->flush)
        drv->flush();
}

/* -------------------------------------------------------------------------- */
/* Input operations                                                           */
/* -------------------------------------------------------------------------- */

int launchpad_vtty_getc(void)
{
    const struct vtty_driver *drv = current_driver();
    if (!drv || !drv->getc)
        return -1;
    return drv->getc();
}

int launchpad_vtty_available(void)
{
    const struct vtty_driver *drv = current_driver();
    if (!drv || !drv->available)
        return 0;
    return drv->available();
}

/* -------------------------------------------------------------------------- */
/* Screen control                                                             */
/* -------------------------------------------------------------------------- */

void launchpad_vtty_clear_screen(void)
{
    const struct vtty_driver *drv = current_driver();
    if (drv && drv->clear_screen)
        drv->clear_screen();
    else
        launchpad_vtty_puts("\x1B[2J");
}

void launchpad_vtty_move_cursor(int row, int col)
{
    const struct vtty_driver *drv = current_driver();
    if (drv && drv->move_cursor)
        drv->move_cursor(row, col);
    else
        launchpad_vtty_printf("\x1B[%d;%dH", row, col);
}

void launchpad_vtty_set_baudrate(int baud)
{
    const struct vtty_driver *drv = current_driver();
    if (drv && drv->set_baudrate)
        drv->set_baudrate(baud);
}

/* -------------------------------------------------------------------------- */
/* Status and extensions                                                      */
/* -------------------------------------------------------------------------- */

int launchpad_vtty_is_ready(void)
{
    const struct vtty_driver *drv = current_driver();
    if (drv && drv->is_ready)
        return drv->is_ready();
    return (drv != NULL);
}

void launchpad_vtty_set_callback(launchpad_vtty_event_cb_t cb)
{
    g_event_cb = cb;
    const struct vtty_driver *drv = current_driver();
    if (drv && drv->set_callback)
        drv->set_callback(cb);
}

int launchpad_vtty_ioctl(int cmd, void *arg)
{
    const struct vtty_driver *drv = current_driver();
    if (!drv || !drv->ioctl)
        return -1;
    return drv->ioctl(cmd, arg);
}

