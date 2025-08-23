#include "launchpad_vtty.h"
#include "driver/uart.h"
#include "freertos/FreeRTOS.h"
#include <string.h>

#ifndef LAUNCHPAD_VTTY_UART_RX_BUF
#define LAUNCHPAD_VTTY_UART_RX_BUF 1024
#endif

struct uart_params {
    int port;
    int tx_pin;
    int rx_pin;
    int baud;
};

static struct uart_params g_uart = {
    .port = 1,
    .tx_pin = 38,
    .rx_pin = 48,
    .baud = 115200,
};

static int uart_init(void)
{
    uart_config_t cfg = {
        .baud_rate = g_uart.baud,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };

    uart_driver_install(g_uart.port, LAUNCHPAD_VTTY_UART_RX_BUF, 0, 0, NULL, 0);
    uart_param_config(g_uart.port, &cfg);
    uart_set_pin(g_uart.port, g_uart.tx_pin, g_uart.rx_pin,
                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    return 0;
}

static int uart_deinit(void)
{
    uart_driver_delete(g_uart.port);
    return 0;
}

static int uart_putc(char c)
{
    uart_write_bytes(g_uart.port, &c, 1);
    return 0;
}

static int uart_puts(const char *s)
{
    if (!s)
        return -1;
    uart_write_bytes(g_uart.port, s, strlen(s));
    return (int)strlen(s);
}

static void uart_flush(void)
{
    uart_wait_tx_done(g_uart.port, portMAX_DELAY);
}

static int uart_getc(void)
{
    uint8_t c;
    int len = uart_read_bytes(g_uart.port, &c, 1, 0);
    return (len > 0) ? c : -1;
}

static int uart_available(void)
{
    size_t len = 0;
    uart_get_buffered_data_len(g_uart.port, &len);
    return (len > 0) ? 1 : 0;
}

static void uart_set_baud(int baud)
{
    g_uart.baud = baud;
    uart_set_baudrate(g_uart.port, baud);
}

static int uart_is_ready(void)
{
    return 1;
}

static const struct vtty_driver uart_driver = {
    .id = 1,
    .type = "uart",
    .init = uart_init,
    .deinit = uart_deinit,
    .putc = uart_putc,
    .puts = uart_puts,
    .flush = uart_flush,
    .getc = uart_getc,
    .available = uart_available,
    .clear_screen = NULL,
    .move_cursor = NULL,
    .set_baudrate = uart_set_baud,
    .is_ready = uart_is_ready,
    .set_callback = NULL,
    .ioctl = NULL,
};

int launchpad_vtty_register_uart(int uart_num, int tx_pin, int rx_pin, int baud)
{
    g_uart.port = uart_num;
    g_uart.tx_pin = tx_pin;
    g_uart.rx_pin = rx_pin;
    g_uart.baud = baud;
    return launchpad_vtty_register_driver(&uart_driver);
}

