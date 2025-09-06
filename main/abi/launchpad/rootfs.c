#include <stdio.h>
#include "esp_littlefs.h"

int launchpad_mount_rootfs()
{
    esp_vfs_littlefs_conf_t bootfs_conf = {
        .base_path = "/root",
        .partition_label = "root",
        .format_if_mount_failed = false,
        .dont_mount = false
    };

    return esp_vfs_littlefs_register(&bootfs_conf) != ESP_OK;
}
