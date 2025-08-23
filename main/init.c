#include <stdint.h>
#include "elf/elf_symbol.h"
#include "elf/esp_elf.h"
#include "platform.h"

void launchpad_init_tty(void) {
    static UARTTTYDriver uartDriver(1, 38, 48, 115200);
    static TTY tty(&uartDriver);
}

void launchpad_init(void) {
    _register_symbol("launchpad_platform", (void *)launchpad_platform);
}
