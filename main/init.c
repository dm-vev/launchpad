#include <stdint.h>
#include "elf/elf_symbol.h"
#include "elf/esp_elf.h"
#include "platform.h"
#include "launchpad_vtty.h"

void launchpad_init(void)
{
    launchpad_vtty_init();

    /* Register vTTY symbols for dynamic lookup */
    _register_symbol("launchpad_vtty_init", (void *)launchpad_vtty_init);
    _register_symbol("launchpad_vtty_deinit", (void *)launchpad_vtty_deinit);
    _register_symbol("launchpad_vtty_register_driver", (void *)launchpad_vtty_register_driver);
    _register_symbol("launchpad_vtty_register_uart", (void *)launchpad_vtty_register_uart);
    _register_symbol("launchpad_vtty_set_default", (void *)launchpad_vtty_set_default);
    _register_symbol("launchpad_vtty_get_current", (void *)launchpad_vtty_get_current);
    _register_symbol("launchpad_vtty_list", (void *)launchpad_vtty_list);
    _register_symbol("launchpad_vtty_putc", (void *)launchpad_vtty_putc);
    _register_symbol("launchpad_vtty_putchar", (void *)launchpad_vtty_putchar);
    _register_symbol("launchpad_vtty_puts", (void *)launchpad_vtty_puts);
    _register_symbol("launchpad_vtty_printf", (void *)launchpad_vtty_printf);
    _register_symbol("launchpad_vtty_flush", (void *)launchpad_vtty_flush);
    _register_symbol("launchpad_vtty_getc", (void *)launchpad_vtty_getc);
    _register_symbol("launchpad_vtty_available", (void *)launchpad_vtty_available);
    _register_symbol("launchpad_vtty_clear_screen", (void *)launchpad_vtty_clear_screen);
    _register_symbol("launchpad_vtty_move_cursor", (void *)launchpad_vtty_move_cursor);
    _register_symbol("launchpad_vtty_set_baudrate", (void *)launchpad_vtty_set_baudrate);
    _register_symbol("launchpad_vtty_is_ready", (void *)launchpad_vtty_is_ready);
    _register_symbol("launchpad_vtty_set_callback", (void *)launchpad_vtty_set_callback);
    _register_symbol("launchpad_vtty_ioctl", (void *)launchpad_vtty_ioctl);

    /* Standard stdio names mapped to vTTY for loaded ELF modules */
    _register_symbol("puts", (void *)launchpad_vtty_puts);
    _register_symbol("printf", (void *)launchpad_vtty_printf);
    _register_symbol("putchar", (void *)launchpad_vtty_putchar);
    _register_symbol("getchar", (void *)launchpad_vtty_getc);

    _register_symbol("launchpad_platform", (void *)launchpad_platform);
}
