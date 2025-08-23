#include <stdio.h>
#include "esp_vfs.h"
#include "esp_vfs_fat.h"
#include "esp_littlefs.h"
#include "exec.h"
#include "init.h"

#define PRINT_FEATURE(mask, name) \
    do { if (g_features & (mask)) printf("  [+] %s\n", name); } while (0)

#define PRINT_HARDWARE(mask, name) \
    do { if (g_hardware & (mask)) printf("  [+] %s\n", name); } while (0)

void app_main(void)
{
    printf("\x1B[1m\x1B[33mStarting LaunchPad...\n");

    // Mount LittleFS on /bootfs
    esp_vfs_littlefs_conf_t bootfs_conf = {
        .base_path = "/boot",
        .partition_label = "boot",
        .format_if_mount_failed = false,
        .dont_mount = false
    };

    if (esp_vfs_littlefs_register(&bootfs_conf) != ESP_OK) {
        printf("Failed to mount bootfs\r\n");
    } else {
        printf("Mounted bootfs at /boot\r\n");
    }

    launchpad_init();

    const char *elf_path = "/boot/app.elf";
    if (!exec_from_file(elf_path, 0, NULL)) {
        printf("ERROR: could not start %s\r\n", elf_path);
    } 

    printf("LaunchPad halted");
    for (;;);
}
