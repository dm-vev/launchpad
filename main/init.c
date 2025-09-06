#include <stdint.h>
#include "elf/elf_symbol.h"
#include "elf/esp_elf.h"
#include "platform.h"
#include "launchpad_vtty.h"

#include "include/log.h"
#include "include/flash.h"
#include "include/sd.h"
#include "include/partition.h"

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

    _register_symbol("launchpad_log", (void *)launchpad_log);

    _register_symbol("launchpad_flash_size",            (void*)launchpad_flash_size);
    _register_symbol("launchpad_flash_erase",           (void*)launchpad_flash_erase);
    _register_symbol("launchpad_flash_erase_range",     (void*)launchpad_flash_erase_range);
    _register_symbol("launchpad_flash_read",            (void*)launchpad_flash_read);
    _register_symbol("launchpad_flash_write",           (void*)launchpad_flash_write);
    _register_symbol("launchpad_flash_write_encrypted", (void*)launchpad_flash_write_encrypted);
    _register_symbol("launchpad_flash_mmap",            (void*)launchpad_flash_mmap);
    _register_symbol("launchpad_flash_munmap",          (void*)launchpad_flash_munmap);
    _register_symbol("launchpad_flash_is_encrypted",    (void*)launchpad_flash_is_encrypted);

    _register_symbol("launchpad_partition_find",          (void*)launchpad_partition_find);
    _register_symbol("launchpad_partition_read",          (void*)launchpad_partition_read);
    _register_symbol("launchpad_partition_write",         (void*)launchpad_partition_write);
    _register_symbol("launchpad_partition_erase_range",   (void*)launchpad_partition_erase_range);
    _register_symbol("launchpad_partition_mmap",          (void*)launchpad_partition_mmap);
    _register_symbol("launchpad_partition_munmap",        (void*)launchpad_partition_munmap);
    _register_symbol("launchpad_partition_verify",        (void*)launchpad_partition_verify);
    _register_symbol("launchpad_partition_is_encrypted",  (void*)launchpad_partition_is_encrypted);

    _register_symbol("launchpad_sd_mount",               (void*)launchpad_sd_mount);
    _register_symbol("launchpad_sd_unmount",             (void*)launchpad_sd_unmount);
    _register_symbol("launchpad_sd_available",           (void*)launchpad_sd_available);
    _register_symbol("launchpad_sd_is_mounted",          (void*)launchpad_sd_is_mounted);
    _register_symbol("launchpad_sd_get_card",            (void*)launchpad_sd_get_card);
    _register_symbol("launchpad_sd_get_free_space_bytes",(void*)launchpad_sd_get_free_space_bytes);
    _register_symbol("launchpad_sd_send_cmd",            (void*)launchpad_sd_send_cmd);

    /* Standard stdio names mapped to vTTY for loaded ELF modules */
    _register_symbol("puts", (void *)launchpad_vtty_puts);
    _register_symbol("printf", (void *)launchpad_vtty_printf);
    _register_symbol("putchar", (void *)launchpad_vtty_putchar);
    _register_symbol("getchar", (void *)launchpad_vtty_getc);

    _register_symbol("launchpad_platform", (void *)launchpad_platform);
}
