#include <stdint.h>
#include "elf/elf_symbol.h"
#include "elf/esp_elf.h"
#include "platform.h"
#include "launchpad_vtty.h"

void launchpad_init(void)
{
    launchpad_vtty_init();
    _register_symbol("launchpad_platform", (void *)launchpad_platform);
}
