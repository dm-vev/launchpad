#ifndef LAUNCHPAD_VTTY_H
#define LAUNCHPAD_VTTY_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Callback type for VTTY events. */
typedef void (*launchpad_vtty_event_cb_t)(int event);

/** Driver interface. */
struct vtty_driver {
    int id;                     /* Driver identifier */
    const char *type;           /* Human readable type */
    int (*init)(void);
    int (*deinit)(void);
    int (*putc)(char c);
    int (*puts)(const char *s);
    void (*flush)(void);
    int (*getc)(void);
    int (*available)(void);
    void (*clear_screen)(void);
    void (*move_cursor)(int row, int col);
    void (*set_baudrate)(int baud);
    int (*is_ready)(void);
    void (*set_callback)(launchpad_vtty_event_cb_t cb);
    int (*ioctl)(int cmd, void *arg);
};

/** Information about a registered VTTY. */
struct launchpad_vtty_info {
    int id;                     /* Driver identifier */
    const char *type;           /* Driver type string */
};

int launchpad_vtty_init(void);
int launchpad_vtty_deinit(void);

int launchpad_vtty_register_driver(const struct vtty_driver *drv);
int launchpad_vtty_register_uart(int uart_num, int tx_pin, int rx_pin, int baud);
int launchpad_vtty_set_default(int id);
struct launchpad_vtty_info launchpad_vtty_get_current(void);
const struct launchpad_vtty_info *launchpad_vtty_list(int *cnt);

int launchpad_vtty_putc(char c);
int launchpad_vtty_putchar(char c);
int launchpad_vtty_puts(const char *s);
int launchpad_vtty_printf(const char *fmt, ...);
void launchpad_vtty_flush(void);

int launchpad_vtty_getc(void);
int launchpad_vtty_available(void);

void launchpad_vtty_clear_screen(void);
void launchpad_vtty_move_cursor(int row, int col);
void launchpad_vtty_set_baudrate(int baud);

int launchpad_vtty_is_ready(void);
void launchpad_vtty_set_callback(launchpad_vtty_event_cb_t cb);
int launchpad_vtty_ioctl(int cmd, void *arg);

#ifdef __cplusplus
}
#endif

#endif /* LAUNCHPAD_VTTY_H */
