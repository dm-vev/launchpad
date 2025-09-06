#include "TTYDriver.hpp"
#include "driver/uart.h"
#include <string.h>

class UARTTTYDriver : public TTYDriver {
private:
    int uart_num;
    int tx_pin;
    int rx_pin;
    int baud;

public:
    UARTTTYDriver(int uart, int tx, int rx, int baudrate)
        : uart_num(uart), tx_pin(tx), rx_pin(rx), baud(baudrate) {}

    void init() override {
        uart_config_t config = {
            .baud_rate = baud,
            .data_bits = UART_DATA_8_BITS,
            .parity    = UART_PARITY_DISABLE,
            .stop_bits = UART_STOP_BITS_1,
            .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
        };

        uart_driver_install((uart_port_t)uart_num, 1024, 0, 0, nullptr, 0);
        uart_param_config((uart_port_t)uart_num, &config);
        uart_set_pin((uart_port_t)uart_num, tx_pin, rx_pin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    }

    void write(const char* str) override {
        uart_write_bytes((uart_port_t)uart_num, str, strlen(str));
    }

    void write_char(char c) override {
        uart_write_bytes((uart_port_t)uart_num, &c, 1);
    }

    char read() override {
        uint8_t c;
        int len = uart_read_bytes((uart_port_t)uart_num, &c, 1, portMAX_DELAY);
        return (len > 0) ? c : 0;
    }
};
